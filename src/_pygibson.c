#include "_pygibson.h"

static PyObject * process_response(gbClient *cl) {
    switch(cl->reply.code) {
        case REPL_OK:
            Py_RETURN_NONE;
        case REPL_VAL:
            return _process_val(&cl->reply);
        case REPL_KVAL:
            return _process_kval(cl);
        default:
            pygibson_set_exception(cl->reply.code, NULL);
            return NULL;
    }
}

static PyObject * _process_val(gbBuffer *buf) {
    long number;
    char *string;
    PyObject *result;
    switch (buf->encoding) {
        case GB_ENC_PLAIN:
#ifdef PYGIBSON_DEBUG
            printf("_process_val(), encoding is GB_ENC_PLAIN, length is %d\n",
                    buf->size);
#endif
            result = PyString_FromStringAndSize(buf->buffer, buf->size);
            return result;
        case GB_ENC_NUMBER:
            number = *(long *)buf->buffer;
#ifdef PYGIBSON_DEBUG
            printf("_process_val(), encoding is GB_ENC_NUMBER, number is %ld\n",
                    number);
#endif
            result = PyLong_FromLong(number);
            return result;
        default:
            pygibson_set_exception(REPL_ERR,
                    "Unknown response encoding");
            return NULL;
   }
}

static PyObject * _process_kval(gbClient *cl) {
#ifdef PYGIBSON_DEBUG
    printf("_process_kval()\n");
#endif
    gbMultiBuffer mb;
    int i=0;
    PyObject *res = PyDict_New();
    if (!res) {
        return NULL;
    }
    gb_reply_multi(cl, &mb);
    for (i=0; i<mb.count; i++) {
#ifdef PYGIBSON_DEBUG
        printf("key '%s' found\n", mb.keys[i]);
#endif
        PyObject *val = _process_val(&mb.values[i]);
        if (val==NULL) {
            gb_reply_multi_free(&mb);
            return NULL;
        }
        if (!PyDict_SetItemString(res, mb.keys[i], val)) {
            gb_reply_multi_free(&mb);
            return NULL;
        }
    }
    gb_reply_multi_free(&mb);
    return res;
}

static gibson_exception * _get_exc(char err_code) {
    gibson_exception *e;
    for (e = py_exceptions; e->name != NULL; e++) {
        if (err_code == e->err_code) return e;
    }
    PyErr_SetString(PyExc_Exception,
            "Enemies everywhere, they are stealing my exceptions!!111");
    return NULL;
}

static void pygibson_set_exception(char err_code, char *message) {
    gibson_exception *e = _get_exc(err_code);
    if (e != NULL) {
        if (message == NULL) message = e->name;
        PyErr_SetString(e->exception, message);
    }
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

