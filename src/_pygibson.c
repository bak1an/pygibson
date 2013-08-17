#include "_pygibson.h"

#ifdef PYGIBSON_DEBUG
#   define DBG printf
#else
#   define DBG(f,...) // x
#endif

#define ERR_BUF_SIZE 1024
static char __gb_error_buffer[ERR_BUF_SIZE] = {0};

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
            DBG("DEBUG: _process_val(), encoding is GB_ENC_PLAIN, length is %d\n",buf->size);
            result = PyString_FromStringAndSize((char *)buf->buffer,
                    buf->size);
            return result;
        case GB_ENC_NUMBER:
            number = gb_reply_number(buf);
            DBG("DEBUG: _process_val(), encoding is GB_ENC_NUMBER, number is %ld\n",number);
            result = PyLong_FromLong(number);
            return result;
        default:
            pygibson_set_exception(REPL_ERR,
                    "Unknown response encoding");
            return NULL;
   }
}

static PyObject * _process_kval(gbClient *cl) {
    DBG("DEBUG: _process_kval()\n");
    gbMultiBuffer mb;
    int i=0;
    PyObject *res = PyDict_New();
    if (!res) {
        return NULL;
    }
    gb_reply_multi(cl, &mb);
    for (i=0; i<mb.count; i++) {
        DBG("DEBUG: key '%s' found\n", mb.keys[i]);
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
        if (message == NULL) {
            if (err_code == REPL_ERR) {
                gb_getlasterror(__gb_error_buffer, ERR_BUF_SIZE);
                message = __gb_error_buffer;
            } else {
                message = e->name;
            }
        }
        PyErr_SetString(e->exception, message);
        DBG("DEBUG: pygibson_set_exception(): setting exception '%s', message is '%s'\n",e->name, message);
    }
}

static void
client_dealloc(client_obj *self) {
    free(self->cl.reply.buffer);
    free(self->cl.request.buffer);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
client_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    client_obj *self;
    self = (client_obj *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->cl.reply.buffer = NULL;
    self->cl.request.buffer = NULL;
    return (PyObject *)self;
}

static int
client_init(client_obj *self, PyObject *args, PyObject *kwds) {
    int port = 0, timeout = 0;
    char *host = NULL, *unix_socket = NULL;
    int connect = 0;
    if (!PyArg_ParseTuple(args, "zizi", &host, &port,
                &unix_socket, &timeout)) {
        return -1;
    }
    if (host == NULL && unix_socket == NULL) {
        pygibson_set_exception(REPL_ERR, "Provide either host:port or unix_socket");
        return -1;
    }
    if (unix_socket == NULL) {
        connect = gb_tcp_connect(&self->cl, host, port, timeout);
        DBG("DEBUG: gb_tcp_connect(): %d\n", connect);
    } else {
        int connect = gb_unix_connect(&self->cl, unix_socket, timeout);
        DBG("DEBUG: gb_unix_connect(): %d\n", connect);
    }
    if (connect != 0) {
        gb_getlasterror(__gb_error_buffer, ERR_BUF_SIZE);
        pygibson_set_exception(REPL_ERR, __gb_error_buffer);
        return -1;
    }
    return 0;
}

// Generic commands:

// generic command that requires key and ttl
static PyObject *
_generic_key_ttl_cmd(client_obj *self, PyObject *args, fp_gb_key_ttl gb_f) {
    char *k;
    int klen, ttl;
    if (!PyArg_ParseTuple(args, "s#I", &k, &klen, &ttl)) {
        return NULL;
    }
    gb_f(&self->cl, k, klen, ttl);
    return process_response(&self->cl);
}

// generic command that requires only key
static PyObject *
_generic_key_cmd(client_obj *self, PyObject *args, fp_gb_key gb_f) {
    char *k;
    int klen;
    if (!PyArg_ParseTuple(args, "s#", &k, &klen)) {
        return NULL;
    }
    gb_f(&self->cl, k, klen);
    return process_response(&self->cl);
}

static PyObject *
_generic_noargs_cmd(client_obj *self, fp_gb_noargs gb_f) {
    gb_f(&self->cl);
    return process_response(&self->cl);
}
// ^^^^^^^^^^^^^^^^
// Generic commands
// ^^^^^^^^^^^^^^^^

static PyObject * cmd_set(client_obj *self, PyObject *args) {
    char *k, *v;
    int klen, vlen, ttl;
    if (!PyArg_ParseTuple(args, "s#s#I", &k, &klen, &v, &vlen, &ttl)) {
        return NULL;
    }
    gb_set(&self->cl, k, klen, v, vlen, ttl);
    return process_response(&self->cl);
}

static PyObject * cmd_mset(client_obj *self, PyObject *args) {
    char *k, *v;
    int klen, vlen;
    if (!PyArg_ParseTuple(args, "s#s#", &k, &klen, &v, &vlen)) {
        return NULL;
    }
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
    return _generic_key_ttl_cmd(self, args, gb_lock);
}

static PyObject * cmd_mlock(client_obj *self, PyObject *args) {
    return _generic_key_ttl_cmd(self, args, gb_mlock);
}

static PyObject * cmd_unlock(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_unlock);
}

static PyObject * cmd_munlock(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_munlock);
}

static PyObject * cmd_count(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_count);
}

static PyObject * cmd_keys(client_obj *self, PyObject *args) {
    return _generic_key_cmd(self, args, gb_keys);
}

static PyObject * cmd_meta(client_obj *self, PyObject *args) {
    char *key, *meta;
    int klen, mlen;
    if (!PyArg_ParseTuple(args, "s#s#", &key, &klen, &meta, &mlen)) {
        return NULL;
    }
    gb_meta(&self->cl, key, klen, meta, mlen);
    return process_response(&self->cl);
}

static PyObject * cmd_stats(client_obj *self) {
    return _generic_noargs_cmd(self, gb_stats);
}

static PyObject * cmd_ping(client_obj *self) {
    return _generic_noargs_cmd(self, gb_ping);
}

static PyObject * cmd_quit(client_obj *self) {
    return _generic_noargs_cmd(self, gb_quit);
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

#define _MOD_NAME "_pygibson"
#define _MOD_DOCSTRING "_pygibson extension"

#ifdef IS_PY3K
    static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        _MOD_NAME,              /* m_name */
        _MOD_DOCSTRING,         /* m_doc */
        -1,                     /* m_size */
        module_methods,         /* m_methods */
        NULL,                   /* m_reload */
        NULL,                   /* m_traverse */
        NULL,                   /* m_clear */
        NULL,                   /* m_free */
    };
    #define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
#else
    #define MOD_INIT(name) PyMODINIT_FUNC init##name(void)
#endif

MOD_INIT(_pygibson) {
    PyObject *m;
    if (PyType_Ready(&client_type) < 0) {
        return;
    }
#ifdef IS_PY3K
    m = PyModule_Create(&moduledef);
#else
    m = Py_InitModule3(_MOD_NAME, module_methods, _MOD_DOCSTRING);
#endif
    Py_INCREF(&client_type);
    PyModule_AddObject(m, "_client", (PyObject *)&client_type);
    _create_exceptions(m);
#ifdef IS_PY3K
    return m;
#endif
}

