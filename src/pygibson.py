from _pygibson import _client
from _pygibson import PyGibsonError
from _pygibson import NotFoundError
from _pygibson import NaNError
from _pygibson import NoMemoryError
from _pygibson import LockedError


class Client(_client):
    def __init__(self, addr="127.0.0.1", port=10128,
                 unix_socket=None, timeout=1000):
        super(Client, self).__init__(addr, port, unix_socket, timeout)

    def set(self, key, value, ttl=0):
        super(Client, self).set(key, value, ttl)

    def keys(self, prefix):
        res = super(Client, self).keys(prefix)
        return list(res.values())
