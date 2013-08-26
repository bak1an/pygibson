# -*- coding: utf-8 -*-

import time
import os
import sys
import pygibson

from . import unittest
from . import ServerSpawningTestCase
from . import _pygibson as pg

PY3 = sys.version_info[0] == 3

if PY3:
    def b(s):
        return s.encode("latin1")
else:
    def b(s):
        return s


class PyGibsonExtensionTest(ServerSpawningTestCase):

    def __init__(self, *args, **kwargs):
        self.config_name = "default.conf"
        super(PyGibsonExtensionTest, self).__init__(*args, **kwargs)

    def setUp(self):
        super(PyGibsonExtensionTest, self).setUp()
        self._cl = pg._client("127.0.0.1", 10128, None, 1000)

    def tearDown(self):
        super(PyGibsonExtensionTest, self).tearDown()
        self._cl = None

    def wait_nan(self, f):
        with self.assertRaises(pg.NaNError):
            f()

    def wait_404(self, f):
        with self.assertRaises(pg.NotFoundError):
            f()

    def wait_locked(self, f):
        with self.assertRaises(pg.LockedError):
            f()

    def test_ping(self):
        self.assertTrue(self._cl.ping() is None)

    def test_set_get(self):
        self._cl.set("key", "val", 600)
        self.assertEqual(self._cl.get("key"), b("val"))
        self.wait_404(lambda: self._cl.get("404"))
        self._cl.set("key2", "val2", 2)
        self.assertEqual(self._cl.get("key2"), b("val2"))
        time.sleep(3)
        self.wait_404(lambda: self._cl.get("key2"))

    def test_mset_mget(self):
        self._cl.set("pref_key", "val", 100)
        self._cl.set("pref_koy", "val2", 100)
        self._cl.set("preef_key", "val3", 100)
        self.assertEqual(self._cl.mset("pref_", "opppa"), 2)
        self.assertEqual(self._cl.get("pref_key"), b("opppa"))
        self.assertEqual(self._cl.get("pref_koy"), b("opppa"))
        self.assertEqual(self._cl.get("preef_key"), b("val3"))
        mget = self._cl.mget("pre")
        self.assertItemsEqual(mget.items(), {
            "pref_key": b("opppa"),
            "pref_koy": b("opppa"),
            "preef_key": b("val3")
        }.items())
        self.wait_404(lambda: self._cl.mget("atatatatta"))
        self.wait_404(lambda: self._cl.mset("atatatatta", "val"))

    def test_ttl_mttl(self):
        self._cl.set("ttl", "val", 1000)
        time.sleep(3)
        self.assertEqual(self._cl.get("ttl"), b("val"))
        self._cl.ttl("ttl", 2)
        time.sleep(3)
        self.wait_404(lambda: self._cl.get("ttl"))
        self._cl.set("TTL1", "val1", 1000)
        self._cl.set("TTL2", "val2", 1000)
        time.sleep(3)
        self.assertEqual(self._cl.get("TTL1"), b("val1"))
        self.assertEqual(self._cl.get("TTL2"), b("val2"))
        self.assertEqual(self._cl.mttl("TTL", 2), 2)
        time.sleep(3)
        self.wait_404(lambda: self._cl.mget("TTL"))

    def test_del_mdel(self):
        self._cl.set("del", "val", 1000)
        self.assertEqual(self._cl.get("del"), b("val"))
        self.assertIsNone(self._cl.dl("del"))
        self.wait_404(lambda: self._cl.get("del"))
        self._cl.set("DEL1", "val1", 1000)
        self._cl.set("DEL2", "val2", 1000)
        self.assertEqual(self._cl.get("DEL1"), b("val1"))
        self.assertEqual(self._cl.get("DEL2"), b("val2"))
        self.assertEqual(self._cl.mdl("DEL"), 2)
        self.wait_404(lambda: self._cl.mget("DEL"))

    def test_inc_dec_minc_mdec(self):
        self._cl.set("some_number", "50", 1000)
        self.assertEqual(self._cl.get("some_number"), b("50"))
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
            "num4": b("xuy")
        }.items())

    def test_stats(self):
        keys = ['reqs_per_client_avg', 'server_time', 'server_allocator',
                'total_items', 'first_item_seen', 'total_cron_done',
                'item_size_avg', 'server_version', 'server_arch',
                'total_compressed_items', 'last_item_seen', 'memory_fragmentation',
                'memory_used', 'server_started', 'total_connections',
                'server_build_datetime', 'memory_peak', 'memory_usable',
                'memory_available', 'total_clients', 'compr_rate_avg', 'total_requests']
        stats = self._cl.stats()
        self.assertTrue(isinstance(stats, dict))
        for k in keys:
            self.assertTrue(k in stats)
        self.assertEqual(len(keys), len(stats.keys()))

    def test_locks(self):
        try:
            # cleanup
            self._cl.munlock("lock")
        except pg.NotFoundError:
            pass
        self._cl.set("lock", "val", 600)
        self._cl.lock("lock", 2)
        self.wait_locked(lambda: self._cl.set("lock", "val2", 600))
        self.assertEqual(self._cl.get("lock"), b("val"))
        time.sleep(3)
        self._cl.set("lock", "val2", 600)
        self.assertEqual(self._cl.get("lock"), b("val2"))
        self._cl.set("lock_num", "100500", 600)
        self._cl.set("lock_num2", "500100", 600)
        self._cl.set("lock_num3", "1", 600)
        self.assertEqual(self._cl.mlock("lock_num", 600), 3)
        self.wait_locked(lambda: self._cl.inc("lock_num"))
        self.wait_locked(lambda: self._cl.dec("lock_num2"))
        self.wait_404(lambda: self._cl.mdec("lock_num"))
        self.wait_404(lambda: self._cl.minc("lock_num"))
        self._cl.unlock("lock_num")
        self.assertEqual(self._cl.minc("lock_num"), 1)
        self.assertEqual(self._cl.inc("lock_num"), 100502)
        self.wait_locked(lambda: self._cl.dec("lock_num2"))
        self.wait_locked(lambda: self._cl.dec("lock_num3"))
        self.assertEqual(self._cl.munlock("lock_num"), 3)  # should not 2 be here?
        self.assertEqual(self._cl.minc("lock_num"), 3)

    def test_count(self):
        try:
            # cleanup
            self._cl.mdl("count")
        except pg.NotFoundError:
            pass
        self._cl.set("count", "val", 600)
        self.assertEqual(self._cl.count("count"), 1)
        self._cl.set("count2", "val2", 600)
        self.assertEqual(self._cl.count("count"), 2)
        self._cl.set("count3", "val3", 600)
        self.assertEqual(self._cl.count("count"), 3)
        # well, counting up to 3 is ok for me

    def test_quit(self):
        some_client = pg._client("127.0.0.1", 10128, None, 1000)
        self.assertIsNone(some_client.ping())
        self.assertIsNone(some_client.quit())
        # silly test

    def test_meta(self):
        allowed_meta = [
            "size", "encoding", "access", "created",
            "ttl", "left", "lock"
        ]
        self._cl.set("meta", "val", 123)
        for m in allowed_meta:
            res = self._cl.meta("meta", m)
            self.assertFalse(res is None)
        self.wait_404(lambda: self._cl.meta("qwerty", "size"))
        with self.assertRaises(pg.PyGibsonError):
            self._cl.meta("meta", "whatsup")

    def test_keys(self):
        self._cl.set("KEYS1", "val", 600)
        self._cl.set("KEYS2", "val1", 600)
        self._cl.set("KEYS3", "val2", 600)
        self.assertItemsEqual(self._cl.keys("KEY").items(), {
            "0": b("KEYS1"),
            "1": b("KEYS2"),
            "2": b("KEYS3")
        }.items())

    def test_bin_data(self):
        data = b('\xfd\xbb{\xbc\xfa[\xe5\xfatI#\x00\xdbE;\x89A@\xc8)')
        key = 'Водка'
        self._cl.set("BIN1", data, 600)
        self._cl.set(key, "atata", 600)
        self.assertEqual(self._cl.get("BIN1"), data)
        self.assertEqual(self._cl.get(key), b("atata"))
        self.assertEqual(self._cl.mget(key).items(), {key: b('atata')}.items())
        self._cl.set(data, data, 600)
        self.assertEqual(self._cl.get(data), data)
        self.assertEqual(self._cl.mget(data).items(),
                         {data: data}.items())


class TestClient(ServerSpawningTestCase):

    def __init__(self, *args, **kwargs):
        self.config_name = "default.conf"
        super(TestClient, self).__init__(*args, **kwargs)

    def test_default_ttl(self):
        c = pygibson.Client()
        c.set("client_ttl", "val")
        self.assertEqual(c.meta("client_ttl", "ttl"), -1)

    def test_keys(self):
        c = pygibson.Client()
        c.set("KEYS1", "val", 600)
        c.set("KEYS2", "val1", 600)
        c.set("KEYS3", "val2", 600)
        keys = c.keys("KEY")
        self.assertTrue(isinstance(keys, list))
        self.assertItemsEqual(keys, [b("KEYS1"), b("KEYS2"), b("KEYS3")])

    def test_exceptions(self):
        c = pygibson.Client()
        with self.assertRaises(pygibson.NotFoundError):
            c.mget(os.urandom(128))


class TestUnixSockets(ServerSpawningTestCase):
    def __init__(self, *args, **kwargs):
        self.config_name = "unix_socket.conf"
        super(TestUnixSockets, self).__init__(*args, **kwargs)
        self.client_kwargs['unix_socket'] = '/tmp/test_gibson.sock'

    def test_unix_socket(self):
        c = pygibson.Client(**self.client_kwargs)
        c.set("unix_socket", "val", 600)
        self.assertEqual(c.get("unix_socket"), b("val"))
