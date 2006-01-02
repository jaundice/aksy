#include <Python.h>
#include "structmember.h"

#include <stdio.h>
#include <usb.h>
#include <assert.h>
#include <time.h>
#include "aksyxusb.h"

#define USB_TIMEOUT 2000

extern int z48_sysex_reply_ok(char* sysex_reply);

typedef struct {
    PyObject_HEAD
    PyObject* sysex_id;
    akai_usb_device sampler;
} AkaiSampler;

static void
AkaiSampler_dealloc(AkaiSampler* self)
{
    int rc;

    if (self->sampler)
    {
	rc = aksyxusb_device_close(self->sampler);
	PyMem_Free(self->sampler);
	self->sampler = NULL;
	if (rc == AKSY_USB_CLOSE_ERROR)
	{
	    printf("WARN: Device was not succesfully closed\n");
	}
    }

    Py_XDECREF(self->sysex_id);
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
AkaiSampler_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    AkaiSampler *self;

    self = (AkaiSampler *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->sampler = NULL;
	self->sysex_id = NULL;
    }

    return (PyObject *)self;
}

static int
AkaiSampler_init(AkaiSampler *self, PyObject *args)
{
    int usb_product_id;
    int rc;

    if(!PyArg_ParseTuple(args, "i", &usb_product_id))
    {
        return -1;
    }

    self->sampler = (akai_usb_device)PyMem_Malloc(sizeof(struct akai_usb_device));
    self->sampler->usb_product_id = usb_product_id;

    rc = aksyxusb_device_init(self->sampler);

    if (rc == AKSY_NO_SAMPLER_FOUND)
    {
        // valgrind complaint: invalid read
        PyMem_Free(self->sampler);
        self->sampler = NULL;
        PyErr_Format(PyExc_Exception, "Sampler not found");
	return -1;
    }

    if (rc == AKSY_USB_INIT_ERROR)
    {
        PyMem_Free(self->sampler);
        self->sampler = NULL;
        PyErr_Format(PyExc_Exception, "USB device init failed");
	return -1;
    }

    if (rc == AKSY_TRANSMISSION_ERROR)
    {
        PyMem_Free(self->sampler);
        self->sampler = NULL;
        PyErr_Format(PyExc_Exception, "Akai setup sequence failed");
	return -1;
    }

    self->sysex_id = Py_BuildValue("i", self->sampler->sysex_id);
    Py_INCREF(self->sysex_id);
    return 0;
}

static int
AkaiSampler_reset_usb(AkaiSampler* self, PyObject *args)
{
    int rc;

    rc = aksyxusb_device_reset(self->sampler);
    if (rc < 0) {
        PyErr_Format(PyExc_Exception, "Exeption during USB reset");
	return -1;
    }
    return 0;
}

/* Gets a file from the sampler. Any existing file with the same name will be overwritten */
static PyObject*
AkaiSampler_get(AkaiSampler* self, PyObject* args)
{
    unsigned char *dest, *src;
    int rc;
    char location;
    if (!self->sampler)
    {
        PyErr_Format(PyExc_Exception, "Device is not initialized.");
	return NULL;
    }


    if(!PyArg_ParseTuple(args, "ssb", &src, &dest, &location))
    {
        return NULL;
    }
    else
    {
        /* create get request */
        rc = aksyxusb_device_get(self->sampler, src, dest, location, USB_TIMEOUT);

        if (rc)
        {
            switch(rc)
            {
                case AKSY_FILE_NOT_FOUND:
                    return PyErr_Format(PyExc_Exception, "File not found");
                case AKSY_INVALID_FILENAME:
                    return PyErr_Format(PyExc_Exception, "Exception during transfer: invalid filename");
                case AKSY_TRANSMISSION_ERROR:
                    return PyErr_Format(PyExc_Exception, "Exception during transfer: transmission error");
                case AKSY_SYSEX_ERROR:
                    return PyErr_Format(PyExc_Exception, "Exception during transfer: sysex error");
                default:
                    return PyErr_Format(PyExc_Exception, "Unknown exception during transfer");
            }
        }
        else
        {
            Py_INCREF(Py_None);
            return Py_None;
        }
    }
}

/* uploads a file to the sampler. */
static PyObject*
AkaiSampler_put(AkaiSampler* self, PyObject* args)
{
    unsigned char *src, *dest;
    char location;
    int rc;

    if(!PyArg_ParseTuple(args, "ssb", &src, &dest, &location))
    {
        return NULL;
    }
    else
    {
        rc = aksyxusb_device_put(self->sampler, src, dest, location, USB_TIMEOUT);
        if (rc)
        {
            switch(rc)
            {
                case AKSY_FILE_NOT_FOUND:
                    return PyErr_Format(PyExc_Exception, "Exception before transfer: file not found");
                case AKSY_FILE_STAT_ERROR:
                    return PyErr_Format(PyExc_Exception, "Exception before transfer: could not get file size");
                case AKSY_EMPTY_FILE_ERROR:
                    return PyErr_Format(PyExc_Exception, "Exception before transfer: cowardly refusing to transfer an empty file");
                case AKSY_FILE_READ_ERROR:
                    return PyErr_Format(PyExc_Exception, "Exception during transfer: error reading file");
                case AKSY_INVALID_FILENAME:
                    return PyErr_Format(PyExc_Exception, "Exception during transfer: invalid filename");
                case AKSY_TRANSMISSION_ERROR:
                    return PyErr_Format(PyExc_Exception, "Exception during transfer: transmission error");
                case AKSY_SYSEX_ERROR:
                    return PyErr_Format(PyExc_Exception, "Exception during transfer: sysex error");
                default:
                    return PyErr_Format(PyExc_Exception, aksyx_get_sysex_error_msg(rc));
            }
        }
        else
        {
            Py_INCREF(Py_None);
            return Py_None;
        }
    }
}

static PyObject*
AkaiSampler_execute(AkaiSampler* self, PyObject* args)
{
    PyObject *ret;
    struct byte_array sysex, buffer;
    const int BUFF_SIZE = 4096;
    int bytes_read = 0, rc;

    if(!PyArg_ParseTuple(args, "s#", &sysex.bytes, &sysex.length))
    {
        return NULL;
    }
    else
    {
	buffer.length = BUFF_SIZE;
        buffer.bytes = (char*)PyMem_Malloc(buffer.length * sizeof(char));

        rc = aksyxusb_device_exec_sysex(
            self->sampler, &sysex, &buffer, &bytes_read, USB_TIMEOUT);

        if (rc == AKSY_TRANSMISSION_ERROR)
        {
            ret = PyErr_Format(PyExc_Exception, "Timeout waiting for sysex reply.");
        }
        else
        {
            ret = Py_BuildValue("s#", buffer.bytes, bytes_read);
        }
        PyMem_Free(buffer.bytes);
        return ret;
    }
}

static PyMemberDef AkaiSampler_members[] = {
    {"sysex_id", T_OBJECT_EX, offsetof(AkaiSampler, sysex_id), 0,
     "System exclusive ID"},
    {NULL}
};

static PyMethodDef AkaiSampler_methods[] =
{
    {"_reset", (PyCFunction)AkaiSampler_reset_usb, METH_VARARGS, "Resets USB device and interface."},
    {"_get", (PyCFunction)AkaiSampler_get, METH_VARARGS, "Gets a file from the sampler"},
    {"_put", (PyCFunction)AkaiSampler_put, METH_VARARGS, "Puts a file on the sampler"},
    {"_execute", (PyCFunction)AkaiSampler_execute, METH_VARARGS, "Executes a Sysex string on the sampler"},
    {NULL},
};

static PyTypeObject aksyx_AkaiSamplerType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "aksyx.AkaiSampler",       /*tp_name*/
    sizeof(AkaiSampler),       /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)AkaiSampler_dealloc, /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Akai USB Sampler",        /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    AkaiSampler_methods,       /* tp_methods */
    AkaiSampler_members,       /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)AkaiSampler_init,/* tp_init */
    0,                         /* tp_alloc */
    AkaiSampler_new,           /* tp_new */
};

static PyMethodDef aksyxusb_methods[] = { {NULL} };

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initaksyxusb(void)
{

    PyObject* m;
    PyObject* loc_disk_id;
    PyObject* loc_mem_id;
    PyObject* z48_usb_id;
    PyObject* s56k_usb_id;
    PyObject* mpc4k_usb_id;

    aksyx_AkaiSamplerType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&aksyx_AkaiSamplerType) < 0)
        return;
    Py_INCREF(&aksyx_AkaiSamplerType);
    m = Py_InitModule3("aksyxusb", aksyxusb_methods, "Aksy USB Extension.");

    loc_disk_id = Py_BuildValue("i", LOC_DISK);
    loc_mem_id = Py_BuildValue("i", LOC_MEMORY);

    z48_usb_id = Py_BuildValue("i", Z48_ID);
    s56k_usb_id = Py_BuildValue("i", S56K_ID);
    mpc4k_usb_id = Py_BuildValue("i", MPC4K_ID);

    PyDict_SetItemString(aksyx_AkaiSamplerType.tp_dict, "DISK", loc_disk_id);
    PyDict_SetItemString(aksyx_AkaiSamplerType.tp_dict, "MEMORY", loc_mem_id);

    PyDict_SetItemString(aksyx_AkaiSamplerType.tp_dict, "Z48", z48_usb_id);
    PyDict_SetItemString(aksyx_AkaiSamplerType.tp_dict, "S56K", s56k_usb_id);
    PyDict_SetItemString(aksyx_AkaiSamplerType.tp_dict, "MPC4K", mpc4k_usb_id);

    Py_DECREF(loc_disk_id);
    Py_DECREF(loc_mem_id);

    Py_DECREF(z48_usb_id);
    Py_DECREF(s56k_usb_id);
    Py_DECREF(mpc4k_usb_id);

    PyModule_AddObject(m, "AkaiSampler", (PyObject *)&aksyx_AkaiSamplerType);
}