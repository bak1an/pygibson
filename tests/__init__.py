import _pygibson

import platform

if platform.python_version() < '2.7':
    try:
        unittest = __import__('unittest2')
    except ImportError:
        print "Install unittest2 to run tests on python2.6"
        raise
else:
    import unittest

_cl = _pygibson._client()


class PyGibsonBaseTest(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        self._cl = _cl
        super(PyGibsonBaseTest, self).__init__(*args, **kwargs)

