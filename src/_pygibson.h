#include <Python.h>
#include "libgibsonclient/src/gibson.h"


typedef struct {
    PyObject_HEAD
    gbClient cl;
} client_obj;


static PyMethodDef client_methods[] = {
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
    0,                         /*ob_size*/
    "_pygibson._client",             /*tp_name*/
    sizeof(client_obj), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    client_dealloc,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "don't use this class",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    client_methods,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)client_init,      /* tp_init */
    0,                         /* tp_alloc */
    client_new                 /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL}
};

#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC init_pygibson() {
    PyObject *m;
    client_type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&client_type) < 0) {
        return;
    }
    m = Py_InitModule3("_pygibson", module_methods,
            "_pygibson extension");
    Py_INCREF(&client_type);
    PyModule_AddObject(m, "_client", (PyObject *)&client_type);
}

