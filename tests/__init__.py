import _pygibson

import os
import platform
import subprocess
import time
from distutils.spawn import find_executable


if platform.python_version() < '2.7':
    try:
        unittest = __import__('unittest2')
    except ImportError:
        print "Install unittest2 to run tests on python2.6"
        raise
else:
    import unittest


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
        if not self.gibson_exec:
            raise Exception("No gibson executable found on PATH")
        if not self.config_path:
            raise Exception("Wrong config specified '%s'" % self.config_name)
        super(ServerSpawningTestCase, self).__init__(*args, **kwargs)

    def _getid(self):
        return '.'.join(self.id().split('.')[2:])

    def _run_gibson(self):
        print "TESTSUITE [%s]: Starting gibson server..." % self._getid()
        args = [self.gibson_exec, "-c", self.config_path]
        self._p = subprocess.Popen(args)
        print "TESTSUITE [%s]: gibson pid is '%d'" % (self._getid(), self._p.pid)

    def _stop_gibson(self):
        if self._p:
            print "TESTSUITE [%s]: Stopping gibson server..." % self._getid()
            self._p.terminate()
            self._p.wait()
            print "TESTSUITE [%s]: Stopped!" % self._getid()

    def setUp(self):
        self._run_gibson()
        time.sleep(0.5)
        super(ServerSpawningTestCase, self).setUp()

    def tearDown(self):
        self._stop_gibson()
        super(ServerSpawningTestCase, self).tearDown()
