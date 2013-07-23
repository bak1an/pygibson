#include "_pygibson.h"
#include <string.h>

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
    PyObject *result;
    switch (buf->encoding) {
        case GB_ENC_PLAIN:
#ifdef PYGIBSON_DEBUG
            printf("DEBUG: _process_val(), encoding is GB_ENC_PLAIN, length is %d\n",
                    buf->size);
#endif
            result = PyString_FromStringAndSize((char *)buf->buffer,
                    buf->size);
            return result;
        case GB_ENC_NUMBER:
            number = gb_reply_number(buf);
#ifdef PYGIBSON_DEBUG
            printf("DEBUG: _process_val(), encoding is GB_ENC_NUMBER, number is %ld\n",
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
    printf("DEBUG: _process_kval()\n");
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
        printf("DEBUG: key '%s' found\n", mb.keys[i]);
#endif
        PyObject *val = _process_val(&mb.values[i]);
        if (val==NULL) {
            gb_reply_multi_free(&mb);
            return NULL;
        }
        if (PyDict_SetItemString(res, mb.keys[i], val) == -1) {
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
#ifdef PYGIBSON_DEBUG
        printf("DEBUG: pygibson_set_exception(): setting exception '%s', message is '%s'\n",
                e->name, message);
#endif
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

// Generic commands:

// generic command that requires key and ttl
static PyObject *
_generic_key_ttl_cmd(client_obj *self, PyObject *args, fp_gb_key_ttl gb_f) {
    char *k;
    int klen, ttl;
    if (!PyArg_ParseTuple(args, "sI", &k, &ttl)) {
        return NULL;
    }
    klen = strlen(k);
    gb_f(&self->cl, k, klen, ttl);
    return process_response(&self->cl);
}

// generic command that requires only key
static PyObject *
_generic_key_cmd(client_obj *self, PyObject *args, fp_gb_key gb_f) {
    char *k;
    int klen;
    if (!PyArg_ParseTuple(args, "s", &k)) {
        return NULL;
    }
    klen = strlen(k);
    gb_f(&self->cl, k, klen);
    return process_response(&self->cl);
}
// ^^^^^^^^^^^^^^^^
// Generic commands
// ^^^^^^^^^^^^^^^^

static PyObject * cmd_set(client_obj *self, PyObject *args) {
    char *k, *v;
    int klen, vlen, ttl;
    if (!PyArg_ParseTuple(args, "ssI", &k, &v, &ttl)) {
        return NULL;
    }
    klen = strlen(k);
    vlen = strlen(v);
    gb_set(&self->cl, k, klen, v, vlen, ttl);
    return process_response(&self->cl);
}

static PyObject * cmd_mset(client_obj *self, PyObject *args) {
    char *k, *v;
    int klen, vlen;
    if (!PyArg_ParseTuple(args, "ss", &k, &v)) {
        return NULL;
    }
    klen = strlen(k);
    vlen = strlen(v);
    gb_mset(&self->cl, k, klen, v, vlen);
    return process_response(&self->cl);
}

static PyObject * cmd_ttl(client_obj *self, PyObject *args) {
    return _generic_key_ttl_cmd(self, args, gb_ttl);
}

static PyObject * cmd_mttl(client_obj *self, PyObject *args) {
    return _generic_key_ttl_cmd(self, args, gb_mttl);
}

static PyObject * cmd_get(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_get);
}

static PyObject * cmd_mget(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_mget);
}

static PyObject * cmd_del(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_del);
}

static PyObject * cmd_mdel(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_mdel);
}

static PyObject * cmd_inc(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_inc);
}

static PyObject * cmd_minc(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_minc);
}

static PyObject * cmd_mdec(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_mdec);
}

static PyObject * cmd_dec(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_dec);
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

static PyObject * cmd_meta(client_obj *self, PyObject *args) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static PyObject * cmd_stats(client_obj *self) {
    gb_stats(&self->cl);
    return process_response(&self->cl);
}

static PyObject * cmd_ping(client_obj *self) {
    gb_ping(&self->cl);
    return process_response(&self->cl);
}

static PyObject * cmd_quit(client_obj *self) {
    PyErr_SetString(PyExc_NotImplementedError, "not implemented yet");
    return NULL;
}

static void _create_exceptions(PyObject *module) {
    char exc_name[64];
    gibson_exception *exc = py_exceptions;
    sprintf(exc_name, "_pygibson.%s", exc->name);
    PyObject *generic = PyErr_NewException(exc_name, NULL, NULL);
    exc->exception = generic;
    PyModule_AddObject(module, exc->name, generic);
    exc++;
    while (exc->name != NULL) {
        sprintf(exc_name, "_pygibson.%s", exc->name);
        PyObject *e = PyErr_NewException(exc_name, generic, NULL);
        exc->exception = e;
        PyModule_AddObject(module, exc->name, e);
        exc++;
    }
}

#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC init_pygibson() {
    PyObject *m;
    if (PyType_Ready(&client_type) < 0) {
        return;
    }
    m = Py_InitModule3("_pygibson", module_methods,
            "_pygibson extension");
    Py_INCREF(&client_type);
    PyModule_AddObject(m, "_client", (PyObject *)&client_type);
    _create_exceptions(m);
}

