# Compiled uartremote library for ESP8266

In this directory you find the latest version of uartremote compiled for ESP8266.

## Mpy library import.
You can copy `uartremote.mpy` to the ESP8266 module using the WebREPL. This is the easiest way to install it. It will take about 4kb of memory.

The file is the result of running this command in the LMS-uart-esp/Libraries/UartRemote/MicroPython/ESP8266 directory:
`mpy-cross -march=xtensa ../uartremote.py -o uartremote.mpy`

## Firmware
We also compiled the Micropython firmware with the `uartremote` compiled as frozen module. This has the advantage that the module uses far less RAM memory (1kb vs 4kb) as compared to loading the `uartremote.mpy` module from flash.

All that is needed, is to copyt the `uartremote.py` file in de `/ports/esp8266/modules` directory and build the firmware. The firmware `micropython-v1.15-uartremote.bin` is version MicroPython v1.15 and can be flashed on the ESP8266 board, as described in the Wiki.
