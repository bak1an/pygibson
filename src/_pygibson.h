#include <Python.h>
#include "libgibsonclient/src/gibson.h"

typedef struct _client_obj {
    PyObject_HEAD
    gbClient cl;
} client_obj;

typedef struct _gibson_exception {
    char err_code;
    char *name;
    PyObject *exception;
} gibson_exception;

static gibson_exception py_exceptions[] = {
    {REPL_ERR, "PyGibsonError", NULL},
    {REPL_ERR_NOT_FOUND, "NotFoundError", NULL},
    {REPL_ERR_NAN, "NaNError", NULL},
    {REPL_ERR_MEM, "NoMemoryError", NULL},
    {REPL_ERR_LOCKED, "LockedError", NULL},
    {0xFF, NULL, NULL}
};

typedef int (*fp_gb_key_ttl)(gbClient *, char *, int,  int);
typedef int (*fp_gb_key)(gbClient *, char *, int);

// generic command that requires key and ttl
static PyObject * _generic_key_ttl_cmd(client_obj *self, PyObject *args,
        fp_gb_key_ttl gb_f);
// generic command that requires only key
static PyObject * _generic_key_cmd(client_obj *self, PyObject *args,
        fp_gb_key gb_f);

static PyObject * process_response(gbClient *cl);
static PyObject * _process_val(gbBuffer *buf);
static PyObject * _process_kval(gbClient *cl);
static gibson_exception * _get_exc(char err_code);
static void pygibson_set_exception(char err_code, char *message);

static PyObject * cmd_set(client_obj *self, PyObject *args);
static PyObject * cmd_mset(client_obj *self, PyObject *args);
static PyObject * cmd_ttl(client_obj *self, PyObject *args);
static PyObject * cmd_mttl(client_obj *self, PyObject *args);
static PyObject * cmd_get(client_obj *self, PyObject *args);
static PyObject * cmd_mget(client_obj *self, PyObject *args);
static PyObject * cmd_del(client_obj *self, PyObject *args);
static PyObject * cmd_mdel(client_obj *self, PyObject *args);
static PyObject * cmd_inc(client_obj *self, PyObject *args);
static PyObject * cmd_minc(client_obj *self, PyObject *args);
static PyObject * cmd_mdec(client_obj *self, PyObject *args);
static PyObject * cmd_dec(client_obj *self, PyObject *args);
static PyObject * cmd_lock(client_obj *self, PyObject *args);
static PyObject * cmd_mlock(client_obj *self, PyObject *args);
static PyObject * cmd_unlock(client_obj *self, PyObject *args);
static PyObject * cmd_munlock(client_obj *self, PyObject *args);
static PyObject * cmd_count(client_obj *self, PyObject *args);
static PyObject * cmd_meta(client_obj *self, PyObject *args);
static PyObject * cmd_stats(client_obj *self);
static PyObject * cmd_ping(client_obj *self);
static PyObject * cmd_quit(client_obj *self);

static PyMethodDef client_methods[] = {
    {"set", (PyCFunction)cmd_set, METH_VARARGS, "do set"},
    {"mset", (PyCFunction)cmd_mset, METH_VARARGS, "do mset"},
    {"ttl", (PyCFunction)cmd_ttl, METH_VARARGS, "do ttl"},
    {"mttl", (PyCFunction)cmd_mttl, METH_VARARGS, "do mttl"},
    {"get", (PyCFunction)cmd_get, METH_VARARGS, "do get"},
    {"mget", (PyCFunction)cmd_mget, METH_VARARGS, "do mget"},
    {"dl", (PyCFunction)cmd_del, METH_VARARGS,
        "do del, renamed to dl to not clash with python keyword 'del'"},
    {"mdl", (PyCFunction)cmd_mdel, METH_VARARGS,
        "do mdel, see 'dl' docstring"},
    {"inc", (PyCFunction)cmd_inc, METH_VARARGS, "do inc"},
    {"minc", (PyCFunction)cmd_minc, METH_VARARGS, "do minc"},
    {"dec", (PyCFunction)cmd_dec, METH_VARARGS, "do dec"},
    {"mdec", (PyCFunction)cmd_mdec, METH_VARARGS, "do mdec"},
    {"lock", (PyCFunction)cmd_lock, METH_VARARGS, "do lock"},
    {"unlock", (PyCFunction)cmd_unlock, METH_VARARGS, "do unlock"},
    {"mlock", (PyCFunction)cmd_mlock, METH_VARARGS, "do mlock"},
    {"munlock", (PyCFunction)cmd_munlock, METH_VARARGS, "do munlock"},
    {"count", (PyCFunction)cmd_count, METH_VARARGS, "do count"},
    {"meta", (PyCFunction)cmd_meta, METH_VARARGS, "do meta"},
    {"stats", (PyCFunction)cmd_stats, METH_NOARGS, "do stats"},
    {"ping", (PyCFunction)cmd_ping, METH_NOARGS, "do ping"},
    {"quit", (PyCFunction)cmd_quit, METH_NOARGS, "do quit"},
    {NULL}
};

static void
client_dealloc(client_obj *self);

static PyObject *
client_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

static int
client_init(client_obj *self, PyObject *args, PyObject *kwds);

static PyTypeObject client_type = {
    PyObject_HEAD_INIT(NULL)
    0,                           /*ob_size*/
    "_pygibson._client",         /*tp_name*/
    sizeof(client_obj),          /*tp_basicsize*/
    0,                           /*tp_itemsize*/
    (destructor)client_dealloc,  /*tp_dealloc*/
    0,                           /*tp_print*/
    0,                           /*tp_getattr*/
    0,                           /*tp_setattr*/
    0,                           /*tp_compare*/
    0,                           /*tp_repr*/
    0,                           /*tp_as_number*/
    0,                           /*tp_as_sequence*/
    0,                           /*tp_as_mapping*/
    0,                           /*tp_hash */
    0,                           /*tp_call*/
    0,                           /*tp_str*/
    0,                           /*tp_getattro*/
    0,                           /*tp_setattro*/
    0,                           /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,          /*tp_flags*/
    "don't use this class",      /* tp_doc */
    0,                           /* tp_traverse */
    0,                           /* tp_clear */
    0,                           /* tp_richcompare */
    0,                           /* tp_weaklistoffset */
    0,                           /* tp_iter */
    0,                           /* tp_iternext */
    client_methods,              /* tp_methods */
    0,                           /* tp_members */
    0,                           /* tp_getset */
    0,                           /* tp_base */
    0,                           /* tp_dict */
    0,                           /* tp_descr_get */
    0,                           /* tp_descr_set */
    0,                           /* tp_dictoffset */
    (initproc)client_init,       /* tp_init */
    0,                           /* tp_alloc */
    client_new                   /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL}
};

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

