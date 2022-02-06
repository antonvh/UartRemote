.. currentmodule:: protocol
.. _protocol:

Protocol
========

The communication between different UartRemote instances uses a self-described packet format. This section describes the packet format in detail.

Packet format
-------------

When a command with its accompanying values is transmitted over the Uart, the following packet format is used:

+------+---------+-----------+---------+----------+-------+----------+-----+
|start |total len|command len| command |format len| format|   data   |end  |
+======+=========+===========+=========+==========+=======+==========+=====+
| ``<``|  ``ln`` | ``lc``    | ``cmd`` | ``lf``   | ``f`` | ``data`` |``>``|
+------+---------+-----------+---------+----------+-------+----------+-----+

with

* ``ln`` the length of the total packet encoded as a single byte,
* ``lc`` the length of the command string `<cmd>` as a single byte,
* ``cmd`` the command specified as a string,
* ``lf`` the length of the format string
* ``f`` the Format encapsulation to pack the values; This can be ``repr`` for encapsulating arbitrary objects, ``raw`` for no encapsulation, or a Python struct format string.
* ``data`` a number of values encapsulated according to ``f``.

The whole message is sandwiched between a `start` and `end` delimitter ``<`` and ``>``.

When the method::

  ur.send_command('test','repr',[1,2,3,4,5])

is used, the following packet will be transmitted over the line::

  b'<b'\x1c\x04test\x04repr([1, 2, 3, 4, 5],)>'

Format Option 1: python struct.pack
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This option interpretes the Format string `f` as the format string of the `struct.pack/unpack` method (see https://docs.python.org/3/library/struct.html), for example::

  send_command('test_struct','3b3s1f',1,2,3,"ale",1.3).

This is the fastest method (1ms) but is limited to c-types, like int, unsigned int etc...

Format Option 2: repr/pickle
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This uses the string representation of data, ``repr()`` to encode it. Then ``eval()`` is used on the receiving end.
::

  ur.encode('test_command', 'repr', [[1,2],[3,4]])

will be encoded as::

  b'%\x0ctest_command\x04repr([[1, 2], [3, 4]],)'

Here's the power of repr::

  ur.encode('test_command','repr',[[1,2],[3,str],[len,True],[2+3]])

becomes::

  b"W\x0ctest_command\x04repr([[1, 2], [3, <class 'str'>], [<built-in function len>, True], [5]],)"

This is slower (7ms) and incompatible with Arduino but it is more flexible.

Format Option 3: raw bytes
^^^^^^^^^^^^^^^^^^^^^^^^^^

This is the fastest option of all, but you'll have to do your own decoding/encoding.
::

  ur.encode('test_command','raw',b'abcd')

is encoded as::

  b'\x15\x0ctest_command\x03rawabcd'



The format string
^^^^^^^^^^^^^^^^^

The type of `<data>` is given according to the [struct Format characters](https://docs.python.org/3/library/struct.html), of which the most commonly used are shown below:

+------------------+-----------------+-----------------+
| Format character |    type         | number of bytes |
+==================+=================+=================+
| ``b``            | `byte`          | 1               |
+------------------+-----------------+-----------------+
| ``B``            | `unsigned byte` | 1               |
+------------------+-----------------+-----------------+
| ``i``            | `int`           | 4               |
+------------------+-----------------+-----------------+
| ``I``            | `unsigned int`  | 4               |
+------------------+-----------------+-----------------+
| ``f``            | `float`         | 4               |
+------------------+-----------------+-----------------+
| ``d``            | `double`        | 8               |
+------------------+-----------------+-----------------+
| ``s``            | `string[]`      | one per char    |
+------------------+-----------------+-----------------+

example::

  ur.call('mycommand','bb3sb',-3,-2,"aha",120)

Note that struct DOES NOT decode utf-8. You will always get a bytestring on the other side. It uses about 1ms to encode/decode.

Special format strings for other encoding types

* ``repr``: use for a pickle-like serialized string encoding/decoding
* ``raw`` : skip encoding altogether and just pas one raw byte string.

example::

  ur.call('mycommand','repr',[[12,34],[56,78]],"tekst",(1,2,3))

This will get all the python types across, but uses about 7ms to encode/decode.
::

  ur.call('mycommand','raw',b"Raw byte string")

.. note::
    If the encoder fails it resorts to raw bytes by default.


