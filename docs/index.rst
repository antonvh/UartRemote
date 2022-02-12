Welcome to UartRemote documentation!
=======================================


This is a library for robust, near real-time communication between two UART devices. We developed it on python 3.9 with LEGO EV3, SPIKE Prime and other MicroPython (ESP/STM32) modules. The library is available on github: `UartRemote on GitHub <https://github.com/antonvh/UartRemote>`_.
The library has the following properties:

* It is fast enough to read sensor data at 30-50Hz.
* It is fully symmetrical, so master and slave can have the same import.
* It includes a RAW REPL mode to upload code to a slave module. This means you can develop code for both modules in one file.
* It is implemented in MicroPython and Arduino/C code. With arduino code, much higher sensor reading speeds are possible, but flashing is a bit less user friendly.
* The library has a command loop to wait and listen for calls. That loop is customizable and non-blocking so you can add your own code to it.
* The python-struct-like encoding is included in the payload, so the other side always knows how to decode it.
* Compatable with most RS232-TTL 3.3v/5v converter board to further expand i/o possibilities. 
* Remote modiule importing

Usage: you can use all of parts of this library for your own projects. Please give us credits at least. We put a lot spare time in this. You are also welcome to contribute. Please fork and PR.


Contents
********

.. toctree::
   :maxdepth: 1

   uartremote
   installation
   examples
   pinout
   hardware
   protocol
   arduinouartremote



Indices and tables
******************

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

Status
******

.. image:: https://readthedocs.org/projects/uartremote/badge/?version=latest
  :target: https://uartremote.readthedocs.io/en/latest/?badge=latest
  :alt: Documentation Status

