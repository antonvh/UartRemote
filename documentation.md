# Remote UART library API and Architecture Documentation: uartremote.py

Detailed contiuation of uartremote.py library README.md


## Packet format
When a command with its accompanying values is transmitted over the Uart, the following packet format is used:

|delimiter|total len|command len|command|format len| format| data|delimiter|
|---------|---------|-----------|-------|----------|-------|-----|---------|
| `<`      |  `ln`   | `lc`    | `cmd` | `lf`    | `f` | binary data | `>`|

with
- `ln` the length of the total packet encoded as a single byte,
- `lc` the length of the command string `<cmd>` as a single byte,
- `cmd` the command specified as a string,
- `lf` the length of the format string
- `f` the Format encapsulation to pack the values; This can be `repr` for encapsulating arbitrary objects, `raw` for no encapsulation, or a Python struct format string.
- `data` a number of values encapsulated according to `f`.

When the method

`ur.send_command('test','repr',[1,2,3,4,5])`
is used, the following packet will be transmitted over the line:

```b'<b'\x1c\x04test\x04repr([1, 2, 3, 4, 5],)>'```

## Format Option 1: python struct.pack
This option interpretes the Format string `f` as the format string of the `struct.pack/unpack` method (see https://docs.python.org/3/library/struct.html), for example 

```send_command('test_struct','3b3s1f',1,2,3,"ale",1.3)```.

This is the fastest method (1ms) but is limited to c-types, like int, unsigned int etc...

## Format Option 2: repr/pickle
This uses the string representation of data, `repr()` to encode it. Then `eval()` is used on the receiving end.

`ur.encode('test_command', 'repr', [[1,2],[3,4]])`

will be encoded as:

`b'%\x0ctest_command\x04repr([[1, 2], [3, 4]],)'`

Here's the power of repr:

`ur.encode('test_command','repr',[[1,2],[3,str],[len,True],[2+3]])`

becomes

`b"W\x0ctest_command\x04repr([[1, 2], [3, <class 'str'>], [<built-in function len>, True], [5]],)"`

This is slower (7ms) and incompatible with Arduino but it is more flexible.

## Format Option 3: raw bytes
This is the fastest option of all, but you'll have to do your own decoding/encoding.

`ur.encode('test_command','raw',b'abcd')`

is encoded as:

`b'\x15\x0ctest_command\x03rawabcd'`


### `call(<cmd>,[<type>,<data>])`
The `call` method allows the Master to send a command to the Slave. When no values need to be passed with the command, the `<type>` and `<data>` can be omitted.  The `<data>` can be a single value, a string or a list of values. 

The Slave acknowledges a command by sending back an acknowledge command, where the string `ack` is appended to the command, and return values of the function being called are sent back. When an error occurs, the `<cmd>` that is sent back, contains `err` and the value is the type of error.

### The format string
The type of `<data>` is given according to the [struct Format characters](https://docs.python.org/3/library/struct.html), of which the most commonly used are shown below:

| Format character | type | number of bytes |
|---------------------|-------|--------------|
| `b` | byte | 1 |
| `B` | unsigned byte | 1 |
| `i` | int | 4 |
| `I` | unsigned int | 4 |
| `f` | float | 4 |
| `d` | double | 8 |
| `s` | string[] | one per char

example:
`ur.call('mycommand','bb3sb',-3,-2,"aha",120)`

Note that struct DOES NOT decode utf-8. You will always get a bytestring on the other side. It uses about 1ms to encode/decode.

#### Special format strings for other encoding types
- `repr`: use for a pickle-like serialized string encoding/decoding
- `raw` : skip encoding altogether and just pas one raw byte string.

example:

`ur.call('mycommand','repr',[[12,34],[56,78]],"tekst",(1,2,3))`

This will get all the python types across, but uses about 7ms to encode/decode.

`ur.call('mycommand','raw',b"Raw byte string")`

#### If encoding fails
If the encoder fails it resorts to raw bytes by default.


# Library description
### `class UartRemote(port,baudrate=115200,timeout=1000,debug=False,esp32+rx,esp32_tx)`

Constructs a Uart communication class for Uart port `port`. Baudrate and timeout definitions for the Uart port can be changed.  The boolean `debug` allows for debugging this class. For the SPIKE prime the port can be abbreviated as a single character string `"A"`.
### UartRemote Methods

#### `UartRemote.flush()`

Flushes the read buffer, by reading all remaining bytes from the Uart.

#### `UartRemote.available()`

Return a non zero value if there is a received command available. Note: on the SPIKE prime, you should use the `receive_command` or the `execute_command`, always with the parameter `reply=False`, after using the `available()` method.

#### `UartRemote.send_command(command,[ t, data])`

Sends a command `command`. When `t` and `data` are omitted, the corresponding function on the Slave is called with no arguments. Otherwise,`data` is encoded as type `t`, where `command` is a string and `data` is a string or a list of values, or multiple values.

#### `UartRemote.receive_command(wait=True)`

Receives a command and returns a tuple `(<command>, <data>)`.  If there is a failure, the `<command>`  will be equal to `'err'`. If `wait` is True, the methods waits until it receives a command. 

#### `UartRemote.call(command)`
#### `UartRemote.call(command,t, data)`
Combines the send and receive functions as defined above. When `t` and `data` are omitted, a dummy value `\x00` of type `z` will be send. The parameter `data` can be a string, a single value or a list of values. 

#### `UartRemote.execute_command(wait=True,check=True)`

If `wait` is True, this medthods Waits for the reception of a command, otherwise, it immediately starts receiving a command. Is the flasg `check` is True, it checks for errors or for an `ack` of onother command. It then calls the function corresponding with the received command (prior set by `add_command`) and sends back the result of the executed function.

#### `UartRemote.loop()`

Loops the `UartRemote.wait-for_command()` command.

#### `UartRemote.add_command(command_function[,format_string],[name=<name>])`

Adds a command `command` to the dictionary of `UartRemote.commands` together with a function name `command_function`. Optionally, if the `command_function` returns parameters, the `format_string` describes the type of the returned parameters. If the `command_function` does not return a value, the `format_string` is omirted. The dictionary with commands is used by the `UartRemote.wait_for_command()` method to call the function as defined upon receiving a specific command. As an argument the `data` that is received is used.

