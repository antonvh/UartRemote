# NOTE

[![Documentation Status](https://readthedocs.org/projects/uartremote/badge/?version=latest)](https://uartremote.readthedocs.io/en/latest/?badge=latest)
      

We are in the process of moving all the documentation to [UartRemote.readthedocs.io](https://uartremote.readthedocs.io/). In the mean time, the documentation that is provided as README's in this GitHub repository, can be outdated. Once the migration is finished, the GitHub will be cleaned.


# Remote UART library: uartremote.py

This is a library for robust, near real-time communication between two UART devices. We developed it on python 3.9 with LEGO EV3, SPIKE Prime and other MicroPython (ESP/STM32) modules using our [LEGO Breakout](https://github.com/antonvh/LMS-uart-esp/wiki) Wifi boards. The library has the following properties:

- It is fast enough to read sensor data at 30-50Hz. (see the aduino library if higer performace of C is needed)
- It is fully symmetrical, so master and slave can have the same import.
- It includes a RAW REPL mode to upload code to a slave module. This means you can develop code for both modules in one file.
- It is implemented in MicroPython and Arduino/C code. With arduino code, much higher sensor reading speeds are possible, but flashing is a bit less user friendly.
- The library has a command loop to wait and listen for calls. That loop is customizable and non-blocking so you can add your own code to it.
- The C-struct-like encoding is included in the payload, so the other side always knows how to decode it.
- Compatable with most RS232-TTL 3.3v/5v converter board to further expand i/o possibilities.
- Remote modiule importing

Usage: you can use all of parts of this library for your own projects. Please give us credits at least. We put a lot spare time in this. You are also welcome to contribute. Please fork and PR.

# Installation

## EV3
Copy uartremote.py into your project directory or library directory for `uartremote import *`. this also is the same for using in python3 on desktopOS

## Arduino

The same UartRemote library is also implemented for [Arduino](https://github.com/antonvh/UartRemote/tree/master/Arduino/UartRemote).

## Micropython

Uniform library that works on standard MicroPython platforms, including the EV3 and the Spike. See further platforms in the [microPython](MicroPython/README.md) directory and in the [LEGO Breakout Wiki](https://github.com/antonvh/LMS-uart-esp/wiki)

## ESP32

remote module loading via REPL [details here](Libraries/UartRemote/MicroPython/ESP32/README.md) and here [here](https://uartremote.readthedocs.io/en/latest/examples.html)

## STM32(SPIKE Prime and Robot Inventor 51515)
Copy the [installer script](blob/master/MicroPython/SPIKE/install_uartremote.py) in an empty project inside the LEGO app and run the script once. You can discard the script afterwards. Alternativly you can use rshell, below. files in the /projects folder are not removed.

## ESP8266 with rshell or webshell
remote load modules with examples [here](https://uartremote.readthedocs.io/en/latest/examples.html)
on ESP32 Copy the [ESP8266 library](Libraries/UartRemote/MicroPython/ESP8266) to the board with (webREPL)[https://github.com/antonvh/LMS-uart-esp/wiki/Connecting-via-webrepl] or rshell>


Copy the plain .py file or the compiled library .mpy into your device
- You can get rshell via: `pip3 install rshell`
- If you have a *single device* this will connect: `rshell -p $(ls /dev/tty.usb*) -b 115200 --editor nano`
- ...otherwise find your device with: `ls /dev/tty.usb*` 
- use your *own* desired modem/serial: `rshell -p /dev/tty.usbserial-141230 -b 115200 --editor nano`
- Your device is now mounted in rshell> as /pyboard
- Use rshell's cp To copy the library >`cp Libraries/UartRemote/MicroPython/uartremote.py /pyboard/`

Alternativly: you can use some IDE's for GUI File managment, such as [ThonnyIDE](https://thonny.org) (win/osx/raspi) or [Mu IDE](https://codewith.mu) if you cannot use rshell command line.

You can edit /pyboard/boot.py while you're at it, to configure your [wifi connection](https://github.com/antonvh/LMS-uart-esp/wiki/Connecting-via-webrepl) for LEGO/STM32 using a [breakout board](https://antonsmindstorms.com/?s=wifi) 

# Use Cases / Examples

For more examples see [example_scripts.py](example_scripts.py) and further demo and example scripts in the platform directories. also the new documents wiki [here](https://uartremote.readthedocs.io/en/latest/examples.html)

## Communication Master/Slave Robot  - LEGO(STM32) & micropythonESP8266

On the Lego robot(master) your code looks like you may need to change your library directory `mpy-robot-tools`:
```python
from projects.uartremote import *

ur = UartRemote('F')

while True:
    r += 1
    r = r % 360
    ur.call('set_color', 'i', r) # Encode as a struct.pack 'i' integer type.
    ack, fps = ur.call('get_fps')
    print("Running at {}fps on the ESP8266 board".format(fps))
```


The code for the `boot.py` file on the slave esp8266 with neopixle and pixelhelpers libraries and hardware would look like this:
```python
from uartremote import *
from neopixel import NeoPixel
from pixelhelpers import hsl_to_rgb

# Init
ur = UartRemote()
np = NeoPixel(machine.Pin(4), 12)

fps= 0
def get_fps():
    global fps
    return fps

color = (255,0,0)
def set_color(hue):
    global color
    color = hsl_to_rgb(hue, 1.0, 0.5)

# Register commands for remote calling
# You only need a format string for outgoing data
ur.add_command(get_fps,'f')
ur.add_command(set_color)

while True:
    start = utime.ticks_ms()
    try:
        np.fill(color)
        ur.process_uart()
    except KeyboardInterrupt:
        ur.enable_repl_locally()
        raise
    except: # The show must go on
        ur.flush()
    duration = utime.ticks_diff(utime.ticks_ms(), start)
    fps = 1000/duration
```
In this example two functions are defined using add_command `get_fps`, and `set_color`. 

These functions are called each time that `get_fps`, or `set_color` is received as a command from the master (LEGO in this example).  Parameters for the functions are extracted from the command, and return values, are attached to the response.


## Communication Master, micropython  - OpenMV & Lego Robot
On the micropython/OpenMV camera you can use code like this:
```python
from uartremote import *

ur = UartRemote()

while(True):
    img = sensor.snapshot()
    # Your image processing and manipulation code goes here...
    blob_location = [12,24]

    if ur.available(): # Some bytes have come in over serial
        command, value = ur.receive_command()
        if command == 'blob':
            ur.ack_ok(command, blob_location)
```
## Communication Slave, micropython  - LEGO(STM32) & OpenMV Robot
On the Lego robot your code looks like you may need to change your library directory `mpy-robot-tools`:
```python
from projects.uartremote import *

ur = UartRemote('E')

while True:
    ack, payload = ur.call('blob')
    if ack == 'bloback': # Original command + 'ack'
        # payload is a python list object, like the one you sent on the other side
        blob_location = payload 
        # Do something with robot motors
    else:
        # Unexpected thinges happened. Stop.
        print(ack, payload)
        port.A.motor.pwm(0)
        port.B.motor.pwm(0)
```

## Communication Master, micropython  - ESP8266
Here the devices master(ESP8266) the ESP8266 is the one sending the commands 

The code for the `boot.py` file for master esp8266:
```python
def led(v):
    print('led')
    for i in v:
        print(i)
    
def imu():
    return(12.3,11.1,180.0)

def grid(v):
    addr=v
    a=[20,21,22,23,24,25,26,27,28]
    return(a[addr%9])

from uartremote import *
ur=UartRemote()
ur.add_command(led)
ur.add_command(imu,'3f')
ur.add_command(grid,'B')
ur.loop()
```
Here three different example functions are used: `led` which takes a value, but does not return a value, `imu` which returns a value, but does not take a value, and `grid` wich takes a values and returns a value.
In the `add_command` method, the second argument is the `formatstring`, defining the format of the return argument9s).

## Communication Slave Rx Mode  - LEGO(STM32)
On the Lego Robot the following code is used (e.g. on the SPIKE):
```python
from projects.uartremote import *
u=UartRemote("A" debug=True)

```
In *repl console* the following examples result in:

        >>> u.call('led','repr',[1,2,3,4])
        ('ledack', b'ok')
        >>> u.call('imu')
        ('imuack', (12.3, 11.1, 180.0))
        >>> u.call('grid','B',1)
        ('gridack', 21)
        >>> u.call('unknown')
        ('err', 'Command not found: unkown')

it will also be print additional debug assistance to the Lego app console.To further handle this data would be up to your requirements.


The following example is used to convert the strings back to a readable format and output to display (e.g. on the SPIKE):

```python
from utime import sleep_ms
from projects.uartremote import *
serial = UartTTL("A")

if True:
    serial_buffer = serial.read().decode('UTF-8')
    print(seial_buffer)
    sleep_ms(1000)
```

## Communication Master Tx Mode  - LEGO(STM32)

On the master (e.g. Lego SPIKE):
```python
from projects.uartremote import *
u=UartRemote("A")
u.call('imu')
u.call('grid','B',10)
u.call('led','repr',[2,100,100,100]) # use repr for pasing an array
```
In this example two functions are defined `imu`, `led` and `grid`. These functions are called each time that `imu`, `led` or `grid` is received as a command.  Parameters for the functions are extracted from the command, and return values, are attached to the respons.


## Simultaneous sending and receiving

The library allows for simultaneously sending and receiving commands from both sides. Below the code for both sides is shown. In this example we use the Lego and the ESP8266 board.

### On the Lego SPIKE(STM32):
```python
import time
from projects.uartremote import *
    
def imu():
    return(12.3,11.1,180.0)

u=UartRemote("A")
u.add_command(imu,'3f')

t_old=time.ticks_ms()+2000                              # wait 2 seconds before starting
q=u.flush()                                             # flush uart rx buffer
while True:
    if u.available():                                   # check if a command is available
        u.process_uart()
    if time.ticks_ms()-t_old>1000:                      # send a command every second
        t_old=time.ticks_ms()
        print("send led")                               # send 'led' command with data
        print("recv=",u.call('led','repr',[1,2,3,4]))   # use repr for array
```

### On the micropython ESP8266:
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
        u.process_uart()
    if time.ticks_ms()-t_old>1000:              # send a command every second
        t_old=time.ticks_ms()
        print("send imu")
        print("recv=",u.call('imu'))    # send 'imu' command & receive result
```

# Further Documentation on packet structure and API
moved to its own resource in [documentaton.md](documentation.md)

# To do, roadmap
- Add wireless networking using sockets, rfcomm, ble etc...
- Add python3 support for laptops/desktops
