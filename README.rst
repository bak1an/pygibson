  :version: 0.1.0

pygibson
========

Python client for gibson_ cache server.

.. _gibson: http://gibson-db.in/


Which python versions are supported?
------------------------------------

Currently testsuite runs with python 2.6 and 2.7. Python 3.x support is in
progress.

All tests are passing successfully with PyPy 2.0.2 but since pygibson is
implemented as CPython extension it's not recommended to use it with PyPy. We
need to find some workaround to this later.


How to install?
---------------
::

  pip install pygibson

or

::

  git clone https://github.com/bak1an/pygibson.git
  cd pygibson
  git submodule update --init
  python setup.py install


How to use?
-----------


.. code:: python

    from pygibson import Client
    from pygibson import NotFoundError

    cl = Client() # defaults, 127.0.0.1:10128, timeout=60
    cl2 = Client(addr="192.168.1.33", port=4321, timeout=1000) # non defaults
    cl3 = Client(unix_socket="/var/run/gibson.sock", timeout=500) # connects to unix socket with timeout set to 500

    cl.set("some_key", "some_value")
    print cl.get("some_key")  # "some_value" will be printed
    try:
        cl.get("no_such_key")
    except NotFoundError:
        print "Yay, no such key!"


Client() class methods
----------------------

+-------------+-------------------+--------------------------+
| Method name | Parameters        | Description              |
+=============+===================+==========================+
| __init__    | addr="127.0.0.1", | create Client() instance |
|             | port=10128,       |                          |
|             | unix_socket=None, |                          |
|             | timeout=60        |                          |
+-------------+-------------------+--------------------------+
| set         |                   | do set                   |
+-------------+-------------------+--------------------------+
| mset        |                   | do mset                  |
+-------------+-------------------+--------------------------+
| ttl         |                   | do ttl                   |
+-------------+-------------------+--------------------------+
| mttl        |                   | do mttl                  |
+-------------+-------------------+--------------------------+
| get         |                   | do get                   |
+-------------+-------------------+--------------------------+
| mget        |                   | do mget                  |
+-------------+-------------------+--------------------------+
| dl          |                   | do del                   |
+-------------+-------------------+--------------------------+
| mdl         |                   | do mdel                  |
+-------------+-------------------+--------------------------+
| inc         |                   | do inc                   |
+-------------+-------------------+--------------------------+
| minc        |                   | do minc                  |
+-------------+-------------------+--------------------------+
| dec         |                   | do dec                   |
+-------------+-------------------+--------------------------+
| mdec        |                   | do mdec                  |
+-------------+-------------------+--------------------------+
| lock        |                   | do lock                  |
+-------------+-------------------+--------------------------+
| unlock      |                   | do unlock                |
+-------------+-------------------+--------------------------+
| mlock       |                   | do mlock                 |
+-------------+-------------------+--------------------------+
| munlock     |                   | do munlock               |
+-------------+-------------------+--------------------------+
| count       |                   | do count                 |
+-------------+-------------------+--------------------------+
| meta        |                   | do meta                  |
+-------------+-------------------+--------------------------+
| keys        |                   | do keys                  |
+-------------+-------------------+--------------------------+
| stats       |                   | do stats                 |
+-------------+-------------------+--------------------------+
| ping        |                   | do ping                  |
+-------------+-------------------+--------------------------+
| quit        |                   | do quit                  |
+-------------+-------------------+--------------------------+


Exceptions
----------

Here is a list of exceptions which are available in **pygibson** module:

+---------------+------------------------------------------------------------------------+
| Exception     | Description                                                            |
+===============+========================================================================+
| PyGibsonError | Generic error. raised when REPL_ERR received from server               |
|               | or any other error occured (connection refused, timeout, etc)          |
+---------------+------------------------------------------------------------------------+
| NotFoundError | Not found error. Raised when REPL_ERR_NOT_FOUND received from server   |
+---------------+------------------------------------------------------------------------+
| NaNError      | Not a number. Raised from **inc**, **minc**, **dec** and **mdec** when |
|               | incrementing/decrementing not a numerical values                       |
+---------------+------------------------------------------------------------------------+
| NoMemoryError | Raised when server has no free memory for your stuff                   |
+---------------+------------------------------------------------------------------------+
| LockedError   | Raised when values that you are trying to modify are locked            |
+---------------+------------------------------------------------------------------------+


Roadmap
=======

- Python 3
- Connection pool
- Ability to connect multiple servers
- Better pypy support


License
=======

pygibson code is distributed under MIT license conditions, see LICENSE for
details.

pygibson distribution includes bundled copy of libgibsonclient_ library which is written by
Simone Margaritelli and distributed on terms of BSD license.

.. _libgibsonclient: https://github.com/evilsocket/libgibsonclient
