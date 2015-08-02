  :version: 0.2.1

|build_status|_

.. |build_status| image:: https://api.travis-ci.org/bak1an/pygibson.png
.. _build_status: https://travis-ci.org/bak1an/pygibson

pygibson
========

Python client for gibson_ cache server.

.. _gibson: http://gibson-db.in/


Which python versions are supported?
------------------------------------

Currently testsuite runs with python 2.6, 2.7, 3.2 and 3.3.

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

    cl = Client() # defaults, 127.0.0.1:10128, timeout=1000
    cl2 = Client(host="192.168.1.33", port=4321, timeout=1500) # non defaults
    cl3 = Client(unix_socket="/var/run/gibson.sock", timeout=500) # connects to unix socket with timeout set to 500

    cl.set("some_key", "some_value")
    print cl.get("some_key")  # "some_value" will be printed
    try:
        cl.get("no_such_key")
    except NotFoundError:
        print "Yay, no such key!"


Client() class methods
----------------------

+-------------+-------------------+-----------------------------------------------------------------------------------+
| Method name | Parameters        | Description                                                                       |
+=============+===================+===================================================================================+
| __init__    | host="127.0.0.1", | If *unix_socket* is None connects to **host**:**port**,                           |
|             | port=10128,       | otherwise connects to given **unix_socket**. *timeout* parameter                  |
|             | unix_socket=None, | can be used to set timeout for IO operations.                                     |
|             | timeout=1000      | Default is 1000ms.                                                                |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| set_        | key, value, ttl=0 | Required parameters are key and value.                                            |
|             |                   | Optional parameter *ttl* can be used to set entry TTL in seconds.                 |
|             |                   | Default is 0, which means infinite TTL.                                           |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| mset_       | prefix, value     | Set **value** for all keys which are starting                                     |
|             |                   | with **prefix**. This method will raise *NotFoundError* if there are no keys      |
|             |                   | starting with **prefix** at server.                                               |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| ttl_        | key, ttl          | Set TTL in seconds for **key**.                                                   |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| mttl_       | prefix, ttl       | Set TTL in seconds for all keys string with **prefix**.                           |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| get_        | key               | Get value for given **key**.                                                      |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| mget_       | prefix            | Get values for keys starting with **prefix**.                                     |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| dl_         | key               | Delete given **key** from server. Renamed to *dl* to avoid clashes with           |
|             |                   | python's keyword.                                                                 |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| mdl_        | prefix            | Delete keys starting with **prefix** from server.                                 |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| inc_        | key               | Increment **key** by 1.                                                           |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| minc_       | prefix            | Increment keys starting with **prefix** by 1.                                     |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| dec_        | key               | Decrement **key** by 1.                                                           |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| mdec_       | prefix            | Decrement keys starting with **prefix** by 1.                                     |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| lock_       | key, time         | Lock **key** for **time** seconds. Any write operations to that key will fail     |
|             |                   | and *LockedError* will be raised.                                                 |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| unlock_     | key               | Unlock **key**.                                                                   |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| mlock_      | prefix, time      | Lock keys starting with **prefix** for **time** seconds.                          |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| munlock_    | prefix            | Unlock keys starting with **prefix**.                                             |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| count_      | prefix            | Get count of keys starting with **prefix**.                                       |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| meta_       | key, field        | Get useful info about *key*. Allowed values for **field** are: 'size', 'encoding' |
|             |                   | 'access', 'created', 'ttl', 'left', 'lock'. Click method's name in first column   |
|             |                   | for details ;)                                                                    |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| keys_       | prefix            | Get list of keys starting with **prefix**.                                        |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| stats_      |                   | Get useful statistics from server. See gibson protocol docs for details.          |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| ping_       |                   | Ping server. Raise *PyGibsonError* if failed.                                     |
+-------------+-------------------+-----------------------------------------------------------------------------------+
| quit_       |                   | Disconnect.                                                                       |
+-------------+-------------------+-----------------------------------------------------------------------------------+

    :NOTE: values packed in **bytes** are returned from *get*, *mget* and  *keys* under python 3.

.. _set: http://gibson-db.in/command-set.php
.. _mset: http://gibson-db.in/command-mset.php
.. _ttl: http://gibson-db.in/command-ttl.php
.. _mttl: http://gibson-db.in/command-mttl.php
.. _get: http://gibson-db.in/command-get.php
.. _mget: http://gibson-db.in/command-mget.php
.. _dl: http://gibson-db.in/command-del.php
.. _mdl: http://gibson-db.in/command-mdel.php
.. _inc: http://gibson-db.in/command-inc.php
.. _minc: http://gibson-db.in/command-minc.php
.. _mdec: http://gibson-db.in/command-mdec.php
.. _dec: http://gibson-db.in/command-dec.php
.. _lock: http://gibson-db.in/command-lock.php
.. _mlock: http://gibson-db.in/command-mlock.php
.. _unlock: http://gibson-db.in/command-unlock.php
.. _munlock: http://gibson-db.in/command-munlock.php
.. _count: http://gibson-db.in/command-count.php
.. _meta: http://gibson-db.in/command-meta.php
.. _keys: http://gibson-db.in/command-keys.php
.. _stats: http://gibson-db.in/command-stats.php
.. _ping: http://gibson-db.in/command-ping.php
.. _quit: http://gibson-db.in/command-quit.php


Exceptions
----------

Here is a list of exceptions which are available in **pygibson** module:

+---------------+------------------------------------------------------------------------+
| Exception     | Description                                                            |
+===============+========================================================================+
| PyGibsonError | Generic error. raised when REPL_ERR received from server               |
|               | or any other error occurred (connection refused, timeout, etc).        |
|               | All other pygibson exceptions are subclassed from this one             |
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
