#include "_pygibson.h"

static PyObject * _gbBuffer2PyObject(gbBuffer buf) {
    return NULL;
}

static PyObject * _get_exc(char err_code) {
    gibson_exception *e;
    for (e = py_exceptions; e->name != NULL; e++) {
        if (err_code == e->err_code) return e->exception;
    }
    PyErr_SetString(PyExc_Exception,
            "Enemies everywhere, they are stealing my exceptions!!111");
    return NULL;
}

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
#ifdef PYGIBSON_DEBUG
    printf("DEBUG: gb_tcp_connect(): %d\n", connect);
#endif
    return 0;
}

static PyObject * cmd_set(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_mset(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_ttl(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_mttl(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_get(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_mget(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_del(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_mdel(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_inc(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_minc(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_mdec(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_dec(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_lock(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_mlock(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_unlock(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_munlock(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_count(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_sizeof(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_msizeof(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_encof(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_stats(client_obj *self) {
    gbClient *cl = &self->cl;
    int ret = gb_stats(cl);
    Py_RETURN_NONE;
}

static PyObject * cmd_ping(client_obj *self) {
    int ret = gb_ping(&self->cl);
    if (ret) {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

static PyObject * cmd_quit(client_obj *self) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

