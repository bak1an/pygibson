pygibson
========

Python client for `gibson db`_.

.. _`gibson db`: http://gibson-db.in/


What python versions are supported?
-----------------------------------

Currently testsuite runs with python 2.6 and 2.7. Python 3.x support is in
progress.

All tests are passing successfully with PyPy 2.0.2 but since pygibson is
implemented as CPython extension it's not recommended to use it with PyPy. We
need to find some workaround to this.


How to install?
---------------
::

  pip install pygibson


Methods
-------

===========  ====================================  ========================
Method name  Parameters                            Description
===========  ====================================  ========================
__init__     hostname, port, unix_socket, timeout  create Client() instance
===========  ====================================  ========================

Exceptions
----------

=============   =============
Exception       Description
=============   =============
PyGibsonError   Generic error
=============   =============


Whats next?
===========

- python 3
- ability to connect multiple servers
- better pypy support


License
=======

pygibson code is distributed under MIT license conditions, see LICENSE for
details.

pygibson distribution includes bundled copy of libgibsonclient_ library which is written by
Simone Margaritelli and distributed on terms of BSD license.

.. _libgibsonclient: https://github.com/evilsocket/libgibsonclient
