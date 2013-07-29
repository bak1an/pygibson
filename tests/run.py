import sys

from . import unittest

tests_to_run = []

def filter_tests(t):
    if isinstance(t, unittest.TestSuite):
        res = unittest.TestSuite()
        for test in t:
            filtered = filter_tests(test)
            if filtered:
                res.addTest(filtered)
        return res
    else:
        test_id = '.'.join(t.id().split('.')[1:])
        for allowed_test in tests_to_run:
            if test_id.startswith(allowed_test):
                return t
        return None

if __name__ == "__main__":
    tests_to_run = sys.argv[1:]
    discovered = unittest.TestLoader().discover(".", pattern="test_*.py")
    if tests_to_run:
        suite = unittest.TestSuite()
        for t in discovered:
            suite.addTest(filter_tests(discovered))
    else:
        suite = discovered
    unittest.TextTestRunner(verbosity=2).run(suite)

