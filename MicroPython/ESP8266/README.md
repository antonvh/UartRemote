# Compiled uartremote library for ESP8266

In this directory you find the latest version of uartremote compiled for ESP8266.

## esp_flash_config.py tool
We wrote a wrapper around esptool.py and some functions for easily flashing new firmware on the ESP8266 and for configuring the settings of webrepl and wifi. see more here [OTA flashing](OTA/README.md)

```
python3 esp_flash_config.py -h
usage: esp_flash_config.py [-h] [--port PORT] [--detect-port] [--flash <micropython.bin>]
                           [--webrepl <password>] [--wifi <ssid>,<pasword>] [--getip]

Micropython flash/configuration tool for ESP8266

optional arguments:
  -h, --help            show this help message and exit
  --port PORT, -p PORT  Serial port e.g. COM3, /dev/ttyUSB0, etc.
  --detect-port, -d     Show detected port
  --flash <micropython.bin>, -f <micropython.bin>
                        Erase flash and write binary file <micropython.bin> to esp device.
  --webrepl <password>  Configure webrepl using password <password>
  --wifi <ssid>,<pasword>
                        Configure wifi connection <ssids> and password <password>
  --getip, -i           show IP address for AP and STA mode

```

This tool is tested on linux, mac-osx and windows. For using the tool, you need to connect an FTDI-232 serial usb converter to the ESP8266 module. You can read in the [Wiki](https://github.com/antonvh/LMS-uart-esp/wiki/Flashing-esp-i2c-board#ftdi-cable-and-macwindowslinux) how that can be done. 

Each of the different OS-es will enumerate the usb serial port differently. To help you find out the specific name that is used on your system of the serial port used for  connecting with the ESP8266 module, you can use the `--detect-port` or `-d` option of the tool. 

### Typical sequence for flashing and configuration the ESP8266

1 - Request serial port for later usage (use the port name in the `--webrepl` and `--getip` commands):

```python3 esp_flash_config.py -d```

2 - Flash microptyhon binary:

```python3 esp_flash_config.py -f micropython_v1.16_uartremote.bin```

3 - Reset the ESP module and configure webrepl, with `<portname>` the serial port obtained from step 1:

```python3 esp_flash_config.py --webrepl python --port <portname>```

4 - Configure wifi in STA mode (for connecting to a wifi base station), with `<portname>` the serial port obtained from step 1:

```python3 esp_flash_config.py --wifi examplessid,examplepassword --port <portname>```

5 - Request IP addresses for AP and STA mode:

```python3 esp_flash_config.py --getip --port <portname>```

Note: if the `--port` option is ommitted in steps 3, 4, and 5, the tool will tyry to call step 1 first. A change from user to boot mode and back is then neccessary. The tool will indicate when to change mode.

## Firmware `micropython_xx_uartremote.bin`
We also [compiled](https://github.com/antonvh/flash-esp) the Micropython firmware with the `uartremote` compiled as frozen module. The latest MicroPython version can be found i This has the advantage that the module uses far less RAM memory (1kb vs 4kb) as compared to loading the `uartremote.mpy` module from flash.

All that is needed, is to copy the `uartremote.py` file in the `/ports/esp8266/modules` directory and build the firmware. The firmware `micropython-v1.16-uartremote.bin` is version MicroPython v1.16 and can be flashed on the ESP8266 board, as described above or in the Wiki.

## Mpy library import.
You can copy `uartremote.mpy` to the ESP8266 module using the WebREPL. This is the easiest way to install it. It will take about 4kb of memory.

The file is the result of running this command in the LMS-uart-esp/Libraries/UartRemote/MicroPython/ESP8266 directory:
`mpy-cross -march=xtensa ../uartremote.py -o uartremote.mpy`
