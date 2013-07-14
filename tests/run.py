from . import unittest
from . import _cl


if __name__ == "__main__":
    if not _cl.ping():
        raise Exception("Can't ping gibson server.")
    suite = unittest.TestLoader().discover(".", pattern="test_*.py")
    unittest.TextTestRunner(verbosity=2).run(suite)

