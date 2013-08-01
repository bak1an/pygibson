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


Methods
-------

.. csv-table::
    :header: "Method name", "Parameters", "Description"
    :widths: 10, 20, 40

    "__init__", "hostname, port, unix_socket, timeout", "create Client() instance"
    "set", "", "do set"
    "mset", "", "do mset"
    "ttl", "", "do ttl"
    "mttl", "", "do mttl"
    "get", "", "do get"
    "mget", "", "do mget"
    "dl", "", "do del, renamed to dl to not clash with python keyword 'del'"
    "mdl", "", "do mdel, see 'dl' docstring"
    "inc", "", "do inc"
    "minc", "", "do minc"
    "dec", "", "do dec"
    "mdec", "", "do mdec"
    "lock", "", "do lock"
    "unlock", "", "do unlock"
    "mlock", "", "do mlock"
    "munlock", "", "do munlock"
    "count", "", "do count"
    "meta", "", "do meta"
    "keys", "", "do keys"
    "stats", "", "do stats"
    "ping", "", "do ping"
    "quit", "", "do quit"


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


Which features are planned for next releases?
=============================================

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
