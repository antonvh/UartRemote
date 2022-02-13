.. currentmodule:: examples
.. _examples:

Examples
========



.. _examples_basic_uartremote

Basic encoding


.. _examples_load_module:

Remote module loading
---------------------

The ESP32 micropython only has a single REPL prompt working properly on UART0 (the primary UART used for uploading firmware). Consequently, the commands in the `UartRemote` library for starting and stopping the remote REPL do not work and we can not use the remote REPL for uploading Python code to the remote micropython instance. 

A simple solution would be to initate the code on the ESP32 as `main.py` and have it executing upon reset. This does not allow to change the software running on the ESP32 without reprogramming it.

Therefore, we came up with the following idea. By saving the different programs on the ESP32 as seperate modules, the remote side can choose which module it should load. The loaded module populates the list of remote commands with the functions that it implements. After that the remote commands can be `call`-ed by the remote side. In this way the remote side can decide at run time which commands it can execute on the ESP32.

How to remotely load a module?
------------------------------

We illustrate the way this works by giving an example.

The ESP32 runs the following commands in its ``main.py`` program::

  from uartremote import *
  ur=UartRemote()         # initialize uartremote on default uart and default uart pins
  ur.loop()               # start listing for commands received from the remote instance

Furthermore, on the ESP32 we have the following code saved as the module ``test.py``::

  # module test.py

  def led(n,r,g,b):
    # code for turning on led n using color (r,g,b)
    # now we only print the received data
    print(n,r,g,b)

  def collect_data():
    # code for pulling tuple
    return [('ABC',123),('ABC',123.456)]

  def add_commands(ur): # call for adding the functions in this module to UartRemote commands
    ur.add_command(led) # does not return any value
    ur.add_command(read_temp,'i') # returns an integer
    ur.add_command(collect_data,'repr') # returns string

When the module above is imported, the function ``add_commands`` will add the two functions that are defined in this module to the current command set of UartRemote. Therefore, this function should be present in your modules that you want to remotely import.

On the master instance (e.g. the Lego robot, where the ESP32 is connected to port 'A'), we use the following code to remotely import the ``test`` module::

  # code running on remote instance
  from projects.uartremote import *
  ur=UartRemote('A')

  cmds_before=ur.get_remote_commands()
  print('before',cmds_before)

  # remotely import module `test`
  ur.add_module('test')

  cmds_after=ur.get_remote_commands()
  print('new commands:',list(set(cmds_after)-set(cmds_before)))

  ack,val=ur.call('led','4B',1,100,200,150)

  ack,val=ur.call('read_temp')
  print('read_temp',val)

  ack,val=ur.call('collect_data')
  print('collect_data',val)

Running this program gives the following output:

>>> before ['enable repl', 'disable repl', 'echo', 'raw echo', 'module', 'get_num_commands', 'get_nth_command']
>>> new commands: ['read_temp', 'led']
>>> read_temp 37
>>> collect_data [('ABC, 123'),('ABC',123.456)]

and on the ESP32 we see:

>>> 1 100 200 150


