.. currentmodule:: arduinouartremote
.. _arduinouartremote:

######################
UartRemote for Arduino
######################

This library is from a protocol point of view compatible with the MicroPython UartRemote library. It can be used in any Arduino project by adding this whole directory to the Arduino library directory. After importing the `UartRemote` library, an example can be selected form the examples in the Arduino IDE. Because this library is written in pure C++, it is faster by a factor of approximately 100 compared to the MicroPython implementation.

Differences compared to MicroPython implementation
--------------------------------------------------

Because C++ lacks the possibility to generate a function call with a variable number of parameters, a conversion function ``unpack`` was introduced. 

A typical definition of a user defined function that will be called upon receiving a command with its accompanying parameters is shown below:

.. code-block:: cpp

  void led(Arguments args) {
    int r,g,b,n;
    unpack(args,&r,&g,&b,&n);
    Serial.printf("LED on: %d, %d, %d, %d\n", r, g, b, n);
    uartremote.send_command("ledack","B",0); 
  }


Here, the user function takes always a single parameters of type ``Arguments``. To obtain the encoded variables, the ``unpack`` function is called.

The user defined function must always return an acknowledgement. In this case a dummy variable of type ``byte`` is returned.

The following example shows how to return one or more values back.

.. code-block:: cpp

  void add(Arguments args) {
    int a,b;
    unpack(args,&a,&b);
    Serial.printf("sum on: %d, %d\n", a, b);
    int c=a+b;
    uartremote.send_command("imuack","i",c);
  }

In this example the sum of ``a`` and ``b`` is returned as an integer (``i``).

API
---


.. cpp:member:: Arguments UartRemote::receive_command(char* cmd);

  Waits for an incomming command and return the received command in ``cmd`` and returns the format and buffer in the struct ``Arguments``. The struct member ``.error`` has value 0 when no error occur.

.. cpp:member:: void UartRemote::send_command(const char* cmd, const char* format, ...);

Send a command ``cmd`` over the UART where the vaiavle arguments are formatted according to the ``format`` string.

.. cpp:member:: Arguments UartRemote::call(const char* cmd, const char* format, ...);

Calls remotely the function specified by ``cmd`` and returns the result in a struct of type ``Argument``. The struct member ``.error`` equals 1 if an error occured. 

.. cpp:member:: int UartRemote::receive_execute()

Receives a command and executes the corresponding local function with the parameters as received from the command. This is a combination of ``receive_command`` and ``command``. Returns 0 if no errors occurred.

.. cpp:member:: void UartRemote::add_command(const char * cmd,  void (*func)(Arguments) );

Adds a function to the list of commands. In the example below, the function ``tst`` is added:

.. code-block:: cpp

  def tst(Arguments a) {
    ...
  }

  uartremote.add_command("tst",&tst);


.. cpp:member:: void UartRemote::command(const char* cmd, Arguments rcvunpack);

Executes the function by looking up the ``cmd`` string in the internal ``cmds`` array that is filled using the ``add_command`` method and maps function names as sring to the actual function pointers.

.. cpp:member:: int UartRemote::available();

Checks whether a character is available in the UART receive buffer. This can be used for a non-blocking implementation of UartRemote in your own loop.

.. cpp:member:: void UartRemote::flush();

Flushes the UART receive buffer. This can be used if an error is suspected.

Private methods:
^^^^^^^^^^^^^^^^

.. cpp:member:: Arguments UartRemote::pack( unsigned char* buf, const char* format, ...);

Packs the variatic list of arguments according to the ``format`` string in the buffer ``buf``.

.. cpp:member:: unsigned char UartRemote::readserial1()

Reads a single byte from the UART receive buffer.


Arguments struct
----------------

We use a struct ``Arguments`` to store the format string together with the buffer with the unpacked data. The ``friend unpack`` function takes care for the proper unpacking of the buffer according to the format string. The ``.error`` struct member is used for passing error status back.

.. code-block:: cpp

  struct Arguments {
    void* buf;
    const char* fmt;
    int error;
    template<typename... Args> friend void unpack(const Arguments& a, Args... args) {
        struct_unpack(a.buf,a.fmt, args...);
    }
  };


Examples
--------

Below are some code snippets showing to use the Arduino side as a master and as a slave with its counter part written in Python.

.. code-block:: cpp
  
  char cmd[32]; // global temporary storage for command names

  UartRemote uartremote;

  void setup() {
    ...
    uartremote.add_command("led", &led);
    uartremote.add_command("add", &add);
    ...
  }

  void loop() {
    int error = uartremote.receive_execute();
    if (error==1) {
      printf("error in receiving command\n");
    }
  }

With on the Python side the following code:

::

  from uartremote import *
  from utime import sleep_ms
  ur=UartRemote()

  ur.flush()

  while True:
    ack,s=ur.call('add','2i',1,2)
    print("sum = ",s)
    sleep_ms(500)
    ur.call('led','4i',1,2,3,4)
    sleep_ms(500)


The other way round it would look like:

.. code-block:: cpp
  
  char cmd[32]; // global temporary storage for command names

  UartRemote uartremote;

  void setup() {
    ...
  }
  int i=0;
  int s=0;

  void loop() {
    i+=1;
    i%=100;
    args=uartremote.call("led","4i",i+1,i+2,i+3,i+4);
    if (args.error==0) {
      printf("received ledack: %s\n",cmd);
    } else { printf("error from call led\n");}
    delay(1000);
    args=uartremote.call("add","2i",i+1,i+2);
    if (args.error==0) {
      unpack(args,&s);
      printf("Received sumack: %s, sum=%d\n",cmd,s);
    } else { printf("error from call sum\n");}
   delay(1000);
  }


On the Pyhton side we have the following code::

  from uartremote import *
  ur=UartRemote()

    def led(n,r,g,b):
    print('led',n,r,g,b)
    
  def add(a,b):
    print('adding',a,b)
    return(a+b)

  ur.add_command(led)
  ur.add_command(add,'i')

  ur.loop()


Struct library
--------------

We use the ``struct`` library with implemenations of ``pack`` and ``unpack`` supporting Python compatible format strings. The code can be found on `github.com/svperbeast/struct <https://github.com/svperbeast/struct>`_.


