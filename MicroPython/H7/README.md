For use on the [OpenMV](https://docs.openmv.io/openmvcam/quickref.html)

# Installation
1. Copy uartremote.py or [SPIKE/uartremote.mpy](https://github.com/antonvh/UartRemote/tree/master/MicroPython/SPIKE) above to the OpenMV flash drive. The uartremote.mpy speeds startup and saves memory. You may need a external SDcard.
2. Optionally copy main.py to the OpenMV flash drive. This boots in UART repl so you can download your scripts from the SPIKE/Robot Inventor side.

Example using basic face detection script from OpenMV IDE

```Python
# minimal face detction script for OpenMV and Spike/51515
# the following ## replaces the main.py on the OpenMV flash
# this adds shell to the serial, port pins as defined
# M4: UART(3):(Tx/RX) = P4, P5 = PB10, PB11
# M7/H7: UART(1):(Tx/RX) = P1, P0 = PB14, PB15
##import pyb, time
##from machine import UART
##from uos import dupterm
##uart = UART(3, 115200)
##dupterm(uart,2)

MAINPY="""
# Face Detection UART Example main.py for OpenMV micropython
#
import sensor, time, image
from uartremote import *

sensor.reset()
sensor.set_contrast(3)
sensor.set_gainceiling(16)
sensor.set_framesize(sensor.HQVGA)
sensor.set_pixformat(sensor.GRAYSCALE)
ur = UartRemote()
face_cascade = image.HaarCascade("frontalface", stages=25)
clock = time.clock()
H_CENTER = 120
V_CENTER = 80

def location(r):
    # Returns a tuple with the centre of the face detection rectangle
    # Coordinates have sensor centre as 0,0 instead of top left
    c = (r[0] + r[2]//2, r[1] + r[3]//2)
    return (c[0]-H_CENTER, c[1]-V_CENTER)
    face_cascade = image.HaarCascade("frontalface", stages=25)

def get_face_loc():
    img = sensor.snapshot()
    
    objects = img.find_features(face_cascade, threshold=0.75, scale_factor=1.25)
    if objects:
        return location(objects[0])
    else:
        return (0,0)

# Two signed bytes should be enough for values between -120 and 120
ur.add_command(get_face_loc, 'bb')
"""

from projects.uartremote import *
ur = UartRemote('A')
print("UartRemote Library initialized")

ur.repl_activate()
print(ur.repl_run("print('Repl Tested')", raw_paste=False))

download_script = (ur.repl_run(MAINPY))
print("Downloaded script")

ur.repl_run("ur.loop()", reply=False, raw_paste=False)
print("Entered remote listening loop")
ur.flush()

from spike import PrimeHub
myhub = PrimeHub()

while True:
    ack, loc = ur.call('get_face_loc')
    if ack != 'err':
        print('face detected:', loc)
```

