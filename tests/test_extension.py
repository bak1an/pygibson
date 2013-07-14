from . import PyGibsonBaseTest


class PyGibsonExtensionTest(PyGibsonBaseTest):
    def test_ping(self):
        self.assertTrue(self._cl.ping())

