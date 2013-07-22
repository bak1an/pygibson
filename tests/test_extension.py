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
        self._cl.set("key2", "val2", 2)
        self.assertEqual(self._cl.get("key2"), "val2")
        time.sleep(3)
        with self.assertRaises(pg.NotFoundError):
            print self._cl.get("key2")

    def test_mset_mget(self):
        self._cl.set("pref_key", "val", 100)
        self._cl.set("pref_koy", "val2", 100)
        self._cl.set("preef_key", "val3", 100)
        self.assertEqual(self._cl.mset("pref_", "opppa"), 2)
        self.assertEqual(self._cl.get("pref_key"), "opppa")
        self.assertEqual(self._cl.get("pref_koy"), "opppa")
        self.assertEqual(self._cl.get("preef_key"), "val3")
        mget = self._cl.mget("pre")
        self.assertItemsEqual(mget.items(), {
            "pref_key": "opppa",
            "pref_koy": "opppa",
            "preef_key": "val3"
        }.items())
        with self.assertRaises(pg.NotFoundError):
            self._cl.mget("atatatatta")

