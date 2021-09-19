# Remote UART library: uartremote.py

This is a library for robust communication between lego EV3/Spike and other MicroPython modules using the UART.

This is a uniform library that works on standard MicroPython platforms, the EV3 and the Spike. 

## Installation

### LMS-esp-wifi board
Upload the uartremote.py or the compiled `uartremote.mpy` from the [ESP8266](./ESP8266) directory through the WebREPL.

### SPIKE Prime and Robot Inventor 51515
You have three options: 
1) just copy the whole script on top of your program.
2) use the installer in the [SPIKE directory](https://github.com/antonvh/LMS-uart-esp/tree/main/Libraries/UartRemote/MicroPython/SPIKE)
3) Use rshell.

#### rshell
Install the `rshell` Python extension by:
```
pip3 install rshell
```

Hook up the Spike prime Hub and identyfy the Serial port on the PC to which the hub is connected. On Windoew,e.g. COMxx on Mac OSX, /dev/tty.xxxxxx, and on Linux, /dev/ttyACMx.

Start the rshell, and connect to the hub, where `<serial port>` is the port as identified above:

```
rshell
connect serial <serial port>
```

You can now copy the library to the hub by issueing the following `rshell` command:

```
cp <path_to_library>/uartremote.py /pyboard/.
```

Note: On Windows use backslashes (`\`) in the `<path_to_library>`.

Now you can either use the Spike programming environment, or start a repl prompt from the `rshell`:

```
repl
```

### MINDSTORMS EV3
Use the MINDSTORMS VSCode extension to create your project. 
1. Make a new project using the extension icon in the left toolbaar.
2. Copy uartremote.py into your project directory
3. Run your program. The extension will automatically upload all files in the project directory.


## Initialize

Below is an example of how to use this library.

On the slave (ESP8266):

```python
from uartremote import *
u=UartRemote(0)
u.add_command(imu,'f')
u.add_command(led)
u.add_command(grid,'f')
u.loop()
```

In this example two functions are defined `imu`, `led` and `grid`. These functions are called each time that `imu`, `led` or `grid` is received as a command.  Parameters for the functions are extracted from the command, and return values, are attached to the respons. In practice, these commands will be added to the `boot.py` file on the slave.

## Sending commands

On the master (e.g. EV3):
```python
from uartremote import *
u=UartRemote(Port.S1)
u.call('imu')
u.call('grid','B',10)
u.call('led','B',[2,100,100,100])
```
### `call(<cmd>,[<type>,<data>])`
The `call` method allows the Master to send a command to the Slave. When no values need to be passed with the command, the `<type>` and `<data>` can be omitted.  The `<data>` can be a single value, a string or a list of values. 

The Slave acknowledges a command by sending back an acknowledge command, where the string `ack` is appended to the command, and return values of the function being called are sent back. When an error occurs, the `<cmd>` that is sent back, contains `err` and the value is the type of error.

#### The format string
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

# Example application
## Slave code
On the slave, the following code is used;
```python
def led(v):
    print('led')
    for i in v:
        print(i)
    
def imu():
    return([12.3,11.1,180.0])

def grid(v):
    addr=v
    a=[20,21,22,23,24,25,26,27,28]
    return(a[addr%9])

from uartremote import *
u=UartRemote()
u.add_command(led)
u.add_command(imu,'f')
u.add_command(grid,'B')
u.loop()
```
Here three different example functions are used: `led` which takes a value, but does not return a value, `imu` which returns a value, but does not take a value, and `grid` wich takes a values and returns a value.
In the `add_command` method, the second argument is the `formatstring`, defining the format of the return argument9s).

## Master code
On the Master the following code is used:
```python
from uartremote import *
u=Uartremote(Port.S1)
```
In repl the following examples result in:
```
>>> u.call('led','B',[1,2,3,4])
('ledack', [])
>>> u.call('imu')
('imuack', [12.29999923706055, 11.09999847412109, 180.0])
>>> u.call('grid','B',1)
('gridack', [21])
>>> u.call('unknown')
('error', [b'nok'])
```

# Simultaneous sending and receiving

The library allows for simultaneously sending and receiving commands from both sides. Below the code for both sides is shown. In this example we use the EV3 and the ESP8266 board.

### EV3
```python
import time
from uartremote import *
    
def imu():
    return([12.3,11.1,180.0])

u=UartRemote(Port.S1)
u.add_command(imu,'f')

t_old=time.ticks_ms()+2000                      # wait 2 seconds before starting
q=u.flush()                                     # flush uart rx buffer
while True:
    if u.available():                           # check if a command is available
        u.execute_command(wait=False)
    if time.ticks_ms()-t_old>1000:              # send a command every second
        t_old=time.ticks_ms()
        print("send led")                       # send 'led' command with data
        print("recv=",u.call('led','b',[1,2,3,4]))
```

### ESP8266
```python
import time
from uartremote import *
u=UartRemote(0)

def led(v):
    print('led')
    for i in v:
        print(i)

u.add_command(led)


t_old=time.ticks_ms()+2000                      # wait 2 seconds before starting
q=u.flush()                                     # flush uart rx buffer
while True:
    if u.available():                           # check if a command is available
        u.execute_command(wait=False)
    if time.ticks_ms()-t_old>1000:              # send a command every second
        t_old=time.ticks_ms()
        print("send imu")
        print("recv=",u.call('imu'))    # send 'imu' command & receive result
```

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

