import time

from . import unittest
from . import PyGibsonBaseTest
from . import _pygibson as pg


class PyGibsonExtensionTest(PyGibsonBaseTest):

    def wait_nan(self, f):
        with self.assertRaises(pg.NaNError):
            f()

    def wait_404(self, f):
        with self.assertRaises(pg.NotFoundError):
            f()

    def test_ping(self):
        self.assertTrue(self._cl.ping() is None)

    def test_set_get(self):
        self._cl.set("key", "val", 600)
        self.assertEqual(self._cl.get("key"), "val")
        self.wait_404(lambda: self._cl.get("404"))
        self._cl.set("key2", "val2", 2)
        self.assertEqual(self._cl.get("key2"), "val2")
        time.sleep(3)
        self.wait_404(lambda: self._cl.get("key2"))

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
        self.wait_404(lambda: self._cl.mget("atatatatta"))

    def test_ttl_mttl(self):
        self._cl.set("ttl", "val", 1000)
        time.sleep(3)
        self.assertEqual(self._cl.get("ttl"), "val")
        self._cl.ttl("ttl", 2)
        time.sleep(3)
        self.wait_404(lambda: self._cl.get("ttl"))
        self._cl.set("TTL1", "val1", 1000)
        self._cl.set("TTL2", "val2", 1000)
        time.sleep(3)
        self.assertEqual(self._cl.get("TTL1"), "val1")
        self.assertEqual(self._cl.get("TTL2"), "val2")
        self.assertEqual(self._cl.mttl("TTL", 2), 2)
        time.sleep(3)
        self.wait_404(lambda: self._cl.mget("TTL"))

    def test_del_mdel(self):
        self._cl.set("del", "val", 1000)
        self.assertEqual(self._cl.get("del"), "val")
        self.assertIsNone(self._cl.dl("del"))
        self.wait_404(lambda: self._cl.get("del"))
        self._cl.set("DEL1", "val1", 1000)
        self._cl.set("DEL2", "val2", 1000)
        self.assertEqual(self._cl.get("DEL1"), "val1")
        self.assertEqual(self._cl.get("DEL2"), "val2")
        self.assertEqual(self._cl.mdl("DEL"), 2)
        self.wait_404(lambda: self._cl.mget("DEL"))

    def test_inc_dec_minc_mdec(self):
        self._cl.set("some_number", "50", 1000)
        self.assertEqual(self._cl.get("some_number"), "50")
        self.assertEqual(self._cl.inc("some_number"), 51)
        self.assertEqual(self._cl.get("some_number"), 51)
        self.assertEqual(self._cl.dec("some_number"), 50)
        self.assertEqual(self._cl.get("some_number"), 50)
        self._cl.set("some_nan", "xuy", 1000)
        self.wait_nan(lambda: self._cl.inc("some_nan"))
        self.wait_404(lambda: self._cl.minc("some_na"))
        self.wait_nan(lambda: self._cl.dec("some_nan"))
        self.wait_404(lambda: self._cl.mdec("some_na"))
        try:
            # some cleanup
            self._cl.mdl("num")
        except pg.NotFoundError:
            pass
        self._cl.set("num1", "50", 1000)
        self._cl.set("num2", "100", 1000)
        self._cl.set("num3", "150", 1000)
        self.assertEqual(self._cl.minc("num"), 3)
        self.assertItemsEqual(self._cl.mget("num").items(), {
            "num1": 51,
            "num2": 101,
            "num3": 151
        }.items())
        self.assertEqual(self._cl.mdec("num"), 3)
        self.assertItemsEqual(self._cl.mget("num").items(), {
            "num1": 50,
            "num2": 100,
            "num3": 150
        }.items())
        self._cl.set("num4", "xuy", 1000)
        self.assertEqual(self._cl.minc("num"), 3)
        self.assertItemsEqual(self._cl.mget("num").items(), {
            "num1": 51,
            "num2": 101,
            "num3": 151,
            "num4": "xuy"
        }.items())

