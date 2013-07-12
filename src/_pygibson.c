#include "_pygibson.h"

static void
client_dealloc(client_obj *self) {
    self->ob_type->tp_free((PyObject *)self);
}

static PyObject *
client_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    client_obj *self;
    self = (client_obj *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    return (PyObject *)self;
}

static int
client_init(client_obj *self, PyObject *args, PyObject *kwds) {
   int connect = gb_tcp_connect(&self->cl,
           NULL, 0, 3600);
   printf("%d\n", connect);
   return 0;
}

static PyObject * cmd_ping(client_obj *self) {
    int ret = gb_ping(&self->cl);
    if (ret) {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}
