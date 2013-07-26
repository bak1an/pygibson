import _pygibson

import os
import platform
from distutils.spawn import find_executable

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


class ServerSpawningTestCase(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        self.gibson_exec = find_executable("gibson")
        self.config_path = None
        if not hasattr(self, 'client_kwargs'):
            self.client_kwargs = {
                'addr': '127.0.0.1',
                'port': 10128,
                'unix_socket': None
            }
        if hasattr(self, 'config_name') and self.config_name:
            files_dir = os.path.dirname(os.path.abspath(__file__)) + "/files/"
            if os.path.exists(files_dir + self.config_name):
                self.config_path = files_dir + self.config_name
        if (not self.gibson_exec or not self.config_path):
            raise Exception("No gibson executable found on PATH or wrong config specified")
        super(ServerSpawningTestCase, self).__init__(*args, **kwargs)

    def setUp(self):
        super(ServerSpawningTestCase, self).setUp()

    def tearDown(self):
        super(ServerSpawningTestCase, self).tearDown()
