import time

from . import unittest
from . import PyGibsonBaseTest
from . import _pygibson as pg


class PyGibsonExtensionTest(PyGibsonBaseTest):
    def test_ping(self):
        self.assertTrue(self._cl.ping() is None)

    def test_set_get(self):
        self._cl.set("key", "val", 600)
        self.assertEqual(self._cl.get("key"), "val")
        with self.assertRaises(pg.NotFoundError):
            print self._cl.get("404")
        self._cl.set("key2", "val2", 4)
        self.assertEqual(self._cl.get("key2"), "val2")
        time.sleep(5)
        with self.assertRaises(pg.NotFoundError):
            print self._cl.get("key2")

