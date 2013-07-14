from . import unittest
from . import PyGibsonBaseTest


class PyGibsonExtensionTest(PyGibsonBaseTest):
    @unittest.skip('skipped')
    def test_ping(self):
        self.assertTrue(self._cl.ping())

    def test_stats(self):
        self._cl.stats()

