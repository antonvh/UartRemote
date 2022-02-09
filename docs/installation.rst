.. currentmodule:: installation
.. _installation:

############
Installation
############


Micropython
===========

The ``uartremote.py`` needs to be copied to the flash memory of the MicroPython platform. Depending on the platform used, this can be slightly different. Below you will find a description for the installation on the different platforms.

ESP32
=====

We provide an ESP32 firmware image that contains the latest ``uartremote.py`` module integrated as a frozen module in Ste7an's github repository `micropython_ulab_lvgl <https://github.com/ste7anste7an/micropython_ulab_lvgl/tree/main/build>`_.

Flash this firmware using the ``esptool``::

  esptool.py --port <serial_port> erase_flash
  esptool.py --port <serial_port> --baud 921600 write_flash 0x1000 firmware_ESP32_ULAB_LVGL_SPIRAM_<timestamp>.bin
  
where ``<serial_port>`` is the port to which the esp32 is connected, and ``<timestamp>`` the timestamp at which the firmware was build.

For other firmwares, you can manually upload the ``uartremote.py`` file to the flash memory of the ESP32 module using WebREPL, rshell, or one of the IDE's.


LEGO SPIKE Prime and Robot Inventor 51515
=========================================

The SPIKE IDE checks the filesystem of the Spike Prime. If it sees any non-system files in the root directory, it triggeres a firmware update. After the firmware update, the non-system files will be deleted. However, files that reside in the ``/project`` will not be deleted after a firmware update.

Installation of uartremote.py or uartremote.mpy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Open a new Python project in the LEGO Education SPIKE Prime IDE and paste in the content of the file `install_uartremote.py <https://github.com/antonvh/UartRemote/blob/master/MicroPython/SPIKE/install_uartremote.py>`_. This script is automaically build using a GitHub Action and is committed to the ``MicroPython/SPIKE`` director of the github repository. Execute the script. Open the console in the IDE. After executing it should show:

::

[10:28:55.389] > writing uartremote.mpy to folder /projects[10:28:55.584] > Finished writing uartremote.mpy.
[10:28:55.610] > Checking hash.[10:28:55.686] > Hash generated:  <hash>
[10:28:55.704] > Uartremote library written succesfully. Resetting....

The timestanps will be different on your system and `<hash>` will show the sha-256 hash value. Now the uartremote.mpy library is copied to the `/project` directory. You can discard the script afterwards.

To use the library include the following line in your python code::

  from project.uartremote import *

Creating the install file
^^^^^^^^^^^^^^^^^^^^^^^^^

If you have the ``mpy-cross`` cross compile tool installed, just do this in the SPIKE directory:
``./create_install_file.py``

Errors while installing uartremote library
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you see any errors when running the ``install_uartremote.py`` script, these will be related to wrong hashes. 
- If the hash of the base64 decoded string is not the same as the hash of the initial uartremote.mpy file you will see

::

  Failed hash of base64 input : <hash>

- if the hash of the uartremote.mpy file written locally to the hub's filesystem differs from the initial hash you will see:

::

  Failed hash of .mpy on SPIKE: <hash>

In both cases you can try again by copying the 1`install_uartremote.py`1 again in to an empty Lego Spike project and rerun the code.



LEGO EV3
========
Copy `uartremote.py` into your project directory or library directory for `uartremote import *`. this also is the same for using in python3 on desktopOS

ESP8266 with rshell
===================

On the ESP8266 copy the `ESP8266 library <https://github.com/antonvh/UartRemote/blob/master/MicroPython/ESP8266>`_ to the board with webREPL or rshell.

Copy the plain .py file or the compiled library .mpy into your device
* You can get rshell via:

  pip3 install rshell
  
* If you have a *single device* this will connect

::

  rshell -p $(ls /dev/tty.usb*) -b 115200 --editor nano

* ...otherwise find your device with:
  
::
    
  ls /dev/tty.usb* 

* use your *own* desired modem/serial:

::
  
  rshell -p /dev/tty.usbserial-141230 -b 115200 --editor nano

* Your device is now mounted in rshell> as /pyboard
* Use rshell's cp To copy the library:

::

  cp Libraries/UartRemote/MicroPython/uartremote.py /pyboard/

Alternativly: you can use some IDE's for GUI File managment, such as `ThonnyIDE <https://thonny.org>`_ (win/osx/raspi) or `Mu IDE <https://codewith.mu>`_ if you cannot use rshell command line.

You can edit ``/pyboard/boot.py`` while you're at it, to configure your `wifi connection <https://github.com/antonvh/LMS-uart-esp/wiki/Connecting-via-webrepl>`_ for LEGO/STM32 using a `breakout board <https://antonsmindstorms.com/?s=wifi>`_. 



Arduino
=======

The same UartRemote library is also implemented for `Arduino <https://github.com/antonvh/UartRemote/tree/master/Arduino/UartRemote>`_.

