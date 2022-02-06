.. currentmodule:: uartremote
.. _uartremote:

##################
UartRemote Library
##################

This library implements a class with methods that help to set up robust communication between instances running MicroPython which are connected over a UART interface.

The library is available on github:  `UartRemote on GitHub <https://github.com/antonvh/UartRemote>`_.

Platforms
=========

The following platforms are supported:

* Lego EV3
* Lego Mindstroms Robot Inventor 51515
* Lego SPIKE Prime
* Espressif ESP8266
* Espressif ESP32
* Espressif ESP32-S2
* OpenMV H7 and M7 (STM32 chipset)
* MaixPy (K210 chipset)
* Windows
* Mac OSX

These platforms are automatically detected by querying ``sys.platform``. Within ``uartremote.py`` these platforms are defined by the constants: ``_EV3,_SPIKE,_ESP8266,_ESP32,_ESP32_S2,_H7,_K210,_MAC,_WIN32``, respectively. There are small differences in the implementation of MicroPython for these platforms. The ``UartRemote`` takes these into account based on the platform type.

Constructor
===========

.. class:: UartRemote(port=0,baudrate=115200,timeout=1500,debug=False,rx_pin=18,tx_pin=19 )

   Construct a UartRemote object on the port given by:

   - ``port`` identifies a port for using the UART.

   ``port`` is board specific:

     - SPIKE: port = : has one I2S bus with id=2.
     - ESP8266: port = 0.
     - ESP32: Use 1 for UART1.

   The following keyword arguments are supported on all platforms:

     - ``port`` is the hardware uart port used to by `UartRemote` class; for LEGO it indicated the harware port, i.e. "A"
     - ``baudrate`` is the baudrate at which the UART communicates, defaults to 115200
     - ``timeout`` is the timeout (in ms) for waiting for data coming in on the UART in the ``receive_command()``, default to 1500
     - ``debug`` is a Boolean flag to generate debug output, default to False

  Only for ESP32 platform:   

     - ``rx_pin`` is only used for ESP32 and indicates the  pin on which the UART will receive information, default to 18
     - ``tx_pin`` is the pin on which the UART will receive information, default to 19
    
   
Methods
=======

.. method:: UartRemote.flush()

  Flushes the read buffer, by reading all remaining bytes from the Uart.

.. method:: UartRemote.available()

  Return a non zero value if there is a received command available. Note: on the SPIKE prime, you should use the ``receive_command`` or the ``execute_command``, always with the parameter ``reply=False``, after using the ``available()`` method.

.. method:: UartRemote.send_command(command,*type_data)

  Sends a command ``command``. ``*type_data`` are a number of argument that consist of a type defintion ``t``, followed by one ore more variables of the type corresponding with the paramater ``t``.

  For example::

    ur=UartRemote()
    ur.send_command('led_color','4B',n,t,g,b)
    # will encode a command to remotely calls the function ``led_color``
    # where the values of the variables ``n,t,g,b`` are passed to that function.


.. method:: UartRemote.receive_command(wait=True)

  Receives a command and returns a tuple ``(<command>, <data>)``.  If there is a failure, the ``<command>``  will be equal to `'err'`. If ``wait`` is True, the methods waits until it receives a command. 

.. method:: UartRemote.call(command, *type_data,**kwargs)
  
  Sends a command to a remote host that is waiting for a call and will wait until an answer comes back.
  Optionally a parameter ``timeout=...`` for the answer is self.timout, or passable as timeout=...

.. method:: UartRemote.process_uart(self, sleep=-2)

  Processes a remote call if there is any. Upon receiving a remote call, the command is processed and the result is send back by internally calling the ``reply_command`` method. Sleeps for ``sleep`` ms after every listen. This method is only used in a loop and is non-blocking (as not for the sleep period).
 
.. method:: UartRemote.reply_command(self, command, value)

  Processes the received command by calling the function with name `command(value)` and passes the arguments as defined in ``value``. The result of this function call is send back by calling the ``send_command(ack_command,result)`` method with  with the ``ack_command`` is the received command prepended with `ack_` and the result (if any) is the return value of the function formatted according to the functions format string.
 
.. method:: UartRemote.loop()

  This is an endless loop around the ``process_uart`` method, replying on all incoming calls. The slave side instants typically has the following code running::
    
    from uartremote import *
    ur = UartRemote()
    ur.loop() # wait for incoming commands

.. method:: UartRemote.add_module(module_name)

  Sends a command to the other side instructing it to import the module with name ``module_name``. The ``module_name`` argument has type string. After importing the module, the remote side calls the function `<module>.add_commands()`. This is a function that you should add to the modules you want to remotely import. See for usage :ref:`Example load module <examples_load_module>`.


.. method:: UartRemote.add_command(command_function, format="", name=None)

  Adds a command `command` to the dictionary of ``UartRemote.commands`` together with a function name ``command_function``. Optionally, if the ``command_function`` returns any parameters, the ``format_string`` describes the type of the returned parameters. If the ``command_function`` does not return a value, the `format_string` is ommited. The dictionary with commands is used by the ``UartRemote.reply_command()`` method to call the function as defined upon receiving a specific command. As an argument the ``data`` that is received is used.

  Below is an example of how to use the ``add_command`` method::

    def example(a,b):
      # example function receiving two arguments and returning the sum of a and b

      return (a+b)

    ur.add_command(example,'f')

  Here ``ur`` is the instantiation of the ``UartRemote`` class and the function ``example`` will return the sum as type float. 

.. method:: UartRemote.get_remote_commands()

  Returns an array containing the commands available by the remote uartremote. You will see a number of default built-in commands such as `echo`. This method can be used to query the commands that are added by remotely importing a new module.  See for usage :ref:`Example load module <examples_load_module>`.

Helper Methods
==============

The methods below are internally called by the methods listed above. You can use these methods should you like to have more low level control.

.. py:data:: Uartremote.command

  Dictionary with the mapping of command name to corresponding functions.
  

.. method:: UartRemote.encode(command,*typedata)

  Encodes a command ``command``. ``*type_data`` are a number of arguments that consist of a type defintion ``t``, followed by one ore more variables of the type corresponding with the paramater ``t``.

  For example::

    ur=UartRemote()
    ur.encode('led_color','4B',1,2,3,4)

    >>> b'\x11\tled_color\x024B\x01\x02\x03\x04'

.. method:: UartRemote.decode(bytestr)

  Decodes an encoded bytestring ``bytestr`` as a tuple with the command and the parameters. If a command without parameters was encoded, the parameters will be ``None``.

  For example::

    ur=UartRemote()
    ur.decode(b'\x11\tled_color\x024B\x01\x02\x03\x04')

    >>> ('led_color', (1, 2, 3, 4))

.. method:: UartRemote.read_all()
  
  Returns all bytes that are available in the UART receive buffer.

.. method:: UartRemote.force_read(self, size=1, timeout=50)

  Some platforms read too fast from the UART and return 0 or None. This method loops until it receives a valid number of ``size`` bytes within ``timeout`` ms.


  