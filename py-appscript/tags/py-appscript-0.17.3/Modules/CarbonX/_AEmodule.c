
//	Modified by HAS, 2005, 2006

/* =========================== Module _AE =========================== */

#include "Python.h"



#include "carbonxtoolbox.h"

/* Macro to test whether a weak-loaded CFM function exists */
#define PyMac_PRECHECK(rtn) do { if ( &rtn == NULL )  {\
        PyErr_SetString(PyExc_NotImplementedError, \
        "Not available in this shared library/OS version"); \
        return NULL; \
    }} while(0)


#include <Carbon/Carbon.h>

extern PyObject *_AEDescX_New(AEDesc *);
extern PyObject *_AEDescX_NewBorrowed(AEDesc *);
extern int _AEDescX_Convert(PyObject *, AEDesc *);
extern int _AEDescX_ConvertDisown(PyObject *, AEDesc *);


#define AEDescX_New _AEDescX_New
#define AEDescX_NewBorrowed _AEDescX_NewBorrowed
#define AEDescX_Convert _AEDescX_Convert
#define AEDescX_ConvertDisown _AEDescX_ConvertDisown

typedef long refcontype;

static pascal OSErr GenericEventHandler(const AppleEvent *request, AppleEvent *reply, refcontype refcon); /* Forward */

AEEventHandlerUPP upp_GenericEventHandler;

static pascal OSErr GenericCoercionHandler(const AEDesc *fromDesc, DescType toType, refcontype refcon, AEDesc *toDesc); /* Forward */

AECoercionHandlerUPP upp_GenericCoercionHandler;

static pascal Boolean AEIdleProc(EventRecord *theEvent, long *sleepTime, RgnHandle *mouseRgn)
{
	if ( PyOS_InterruptOccurred() )
		return 1;
	return 0;
}

AEIdleUPP upp_AEIdleProc;

static PyObject *AE_Error;

/* ----------------------- Object type AEDesc ----------------------- */

PyTypeObject AEDesc_Type;

#define AEDesc_Check(x) ((x)->ob_type == &AEDesc_Type || PyObject_TypeCheck((x), &AEDesc_Type))

typedef struct AEDescObject {
	PyObject_HEAD
	AEDesc ob_itself;
	int ob_owned;
} AEDescObject;

PyObject *AEDescX_New(AEDesc *itself)
{
	AEDescObject *it;
	it = PyObject_NEW(AEDescObject, &AEDesc_Type);
	if (it == NULL) return NULL;
	it->ob_itself = *itself;
	it->ob_owned = 1;
	return (PyObject *)it;
}
int AEDescX_Convert(PyObject *v, AEDesc *p_itself)
{
	if (!AEDesc_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "CarbonX._AE.AEDesc required");
		return 0;
	}
	*p_itself = ((AEDescObject *)v)->ob_itself;
	return 1;
}

static void AEDesc_dealloc(AEDescObject *self)
{
	if (self->ob_owned) AEDisposeDesc(&self->ob_itself);
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *AEDesc_AECoerceDesc(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	DescType toType;
	AEDesc result;
#ifndef AECoerceDesc
	PyMac_PRECHECK(AECoerceDesc);
#endif
	if (!PyArg_ParseTuple(_args, "O&",
	                      PyMac_GetOSType, &toType))
		return NULL;
	_err = AECoerceDesc(&_self->ob_itself,
	                    toType,
	                    &result);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &result);
	return _res;
}

static PyObject *AEDesc_AEDuplicateDesc(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEDesc result;
#ifndef AEDuplicateDesc
	PyMac_PRECHECK(AEDuplicateDesc);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	_err = AEDuplicateDesc(&_self->ob_itself,
	                       &result);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &result);
	return _res;
}

static PyObject *AEDesc_AECountItems(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	long theCount;
#ifndef AECountItems
	PyMac_PRECHECK(AECountItems);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	_err = AECountItems(&_self->ob_itself,
	                    &theCount);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("l",
	                     theCount);
	return _res;
}

static PyObject *AEDesc_AECheckIsRecord(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	Boolean isRecord;
#ifndef AECheckIsRecord
	PyMac_PRECHECK(AECheckIsRecord);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	isRecord = AECheckIsRecord(&_self->ob_itself);
	_res = Py_BuildValue("b",
	                     isRecord);
	return _res;
}

static PyObject *AEDesc_AEPutPtr(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	long index;
	DescType typeCode;
	char *dataPtr__in__;
	long dataPtr__len__;
	int dataPtr__in_len__;
#ifndef AEPutPtr
	PyMac_PRECHECK(AEPutPtr);
#endif
	if (!PyArg_ParseTuple(_args, "lO&s#",
	                      &index,
	                      PyMac_GetOSType, &typeCode,
	                      &dataPtr__in__, &dataPtr__in_len__))
		return NULL;
	dataPtr__len__ = dataPtr__in_len__;
	_err = AEPutPtr(&_self->ob_itself,
	                index,
	                typeCode,
	                dataPtr__in__, dataPtr__len__);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AEPutDesc(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	long index;
	AEDesc theAEDesc;
#ifndef AEPutDesc
	PyMac_PRECHECK(AEPutDesc);
#endif
	if (!PyArg_ParseTuple(_args, "lO&",
	                      &index,
	                      AEDescX_Convert, &theAEDesc))
		return NULL;
	_err = AEPutDesc(&_self->ob_itself,
	                 index,
	                 &theAEDesc);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AEGetNthPtr(AEDescObject *_self, PyObject *_args) // TO DO: see AEDesc_AEGetAttributePtr
{
	PyObject *_res = NULL;
	OSErr _err;
	long index;
	DescType desiredType;
	AEKeyword theAEKeyword;
	DescType typeCode;
	char *dataPtr__out__;
	long dataPtr__len__;
	int dataPtr__in_len__;
#ifndef AEGetNthPtr
	PyMac_PRECHECK(AEGetNthPtr);
#endif
	if (!PyArg_ParseTuple(_args, "lO&i",
	                      &index,
	                      PyMac_GetOSType, &desiredType,
	                      &dataPtr__in_len__))
		return NULL;
	if ((dataPtr__out__ = malloc(dataPtr__in_len__)) == NULL)
	{
		PyErr_NoMemory();
		goto dataPtr__error__;
	}
	dataPtr__len__ = dataPtr__in_len__;
	_err = AEGetNthPtr(&_self->ob_itself,
	                   index,
	                   desiredType,
	                   &theAEKeyword,
	                   &typeCode,
	                   dataPtr__out__, dataPtr__len__, &dataPtr__len__);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&O&s#",
	                     PyMac_BuildOSType, theAEKeyword,
	                     PyMac_BuildOSType, typeCode,
	                     dataPtr__out__, (int)dataPtr__len__);
	free(dataPtr__out__);
 dataPtr__error__: ;
	return _res;
}

static PyObject *AEDesc_AEGetNthDesc(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	long index;
	DescType desiredType;
	AEKeyword theAEKeyword;
	AEDesc result;
#ifndef AEGetNthDesc
	PyMac_PRECHECK(AEGetNthDesc);
#endif
	if (!PyArg_ParseTuple(_args, "lO&",
	                      &index,
	                      PyMac_GetOSType, &desiredType))
		return NULL;
	_err = AEGetNthDesc(&_self->ob_itself,
	                    index,
	                    desiredType,
	                    &theAEKeyword,
	                    &result);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&O&",
	                     PyMac_BuildOSType, theAEKeyword,
	                     AEDescX_New, &result);
	return _res;
}

static PyObject *AEDesc_AESizeOfNthItem(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	long index;
	DescType typeCode;
	Size dataSize;
#ifndef AESizeOfNthItem
	PyMac_PRECHECK(AESizeOfNthItem);
#endif
	if (!PyArg_ParseTuple(_args, "l",
	                      &index))
		return NULL;
	_err = AESizeOfNthItem(&_self->ob_itself,
	                       index,
	                       &typeCode,
	                       &dataSize);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&l",
	                     PyMac_BuildOSType, typeCode,
	                     dataSize);
	return _res;
}

static PyObject *AEDesc_AEDeleteItem(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	long index;
#ifndef AEDeleteItem
	PyMac_PRECHECK(AEDeleteItem);
#endif
	if (!PyArg_ParseTuple(_args, "l",
	                      &index))
		return NULL;
	_err = AEDeleteItem(&_self->ob_itself,
	                    index);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AEPutParamPtr(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
	DescType typeCode;
	char *dataPtr__in__;
	long dataPtr__len__;
	int dataPtr__in_len__;
#ifndef AEPutParamPtr
	PyMac_PRECHECK(AEPutParamPtr);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&s#",
	                      PyMac_GetOSType, &theAEKeyword,
	                      PyMac_GetOSType, &typeCode,
	                      &dataPtr__in__, &dataPtr__in_len__))
		return NULL;
	dataPtr__len__ = dataPtr__in_len__;
	_err = AEPutParamPtr(&_self->ob_itself,
	                     theAEKeyword,
	                     typeCode,
	                     dataPtr__in__, dataPtr__len__);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AEPutParamDesc(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
	AEDesc theAEDesc;
#ifndef AEPutParamDesc
	PyMac_PRECHECK(AEPutParamDesc);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&",
	                      PyMac_GetOSType, &theAEKeyword,
	                      AEDescX_Convert, &theAEDesc))
		return NULL;
	_err = AEPutParamDesc(&_self->ob_itself,
	                      theAEKeyword,
	                      &theAEDesc);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AEGetParamPtr(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
	DescType desiredType;
	DescType typeCode;
	char *dataPtr__out__;
	long dataPtr__len__;
	int dataPtr__in_len__;
#ifndef AEGetParamPtr
	PyMac_PRECHECK(AEGetParamPtr);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&i",
	                      PyMac_GetOSType, &theAEKeyword,
	                      PyMac_GetOSType, &desiredType,
	                      &dataPtr__in_len__))
		return NULL;
	if ((dataPtr__out__ = malloc(dataPtr__in_len__)) == NULL)
	{
		PyErr_NoMemory();
		goto dataPtr__error__;
	}
	dataPtr__len__ = dataPtr__in_len__;
	_err = AEGetParamPtr(&_self->ob_itself,
	                     theAEKeyword,
	                     desiredType,
	                     &typeCode,
	                     dataPtr__out__, dataPtr__len__, &dataPtr__len__); // TO DO: see AEDesc_AEGetAttributePtr
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&s#",
	                     PyMac_BuildOSType, typeCode,
	                     dataPtr__out__, (int)dataPtr__len__);
	free(dataPtr__out__);
 dataPtr__error__: ;
	return _res;
}

static PyObject *AEDesc_AEGetParamDesc(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
	DescType desiredType;
	AEDesc result;
#ifndef AEGetParamDesc
	PyMac_PRECHECK(AEGetParamDesc);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&",
	                      PyMac_GetOSType, &theAEKeyword,
	                      PyMac_GetOSType, &desiredType))
		return NULL;
	_err = AEGetParamDesc(&_self->ob_itself,
	                      theAEKeyword,
	                      desiredType,
	                      &result);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &result);
	return _res;
}

static PyObject *AEDesc_AESizeOfParam(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
	DescType typeCode;
	Size dataSize;
#ifndef AESizeOfParam
	PyMac_PRECHECK(AESizeOfParam);
#endif
	if (!PyArg_ParseTuple(_args, "O&",
	                      PyMac_GetOSType, &theAEKeyword))
		return NULL;
	_err = AESizeOfParam(&_self->ob_itself,
	                     theAEKeyword,
	                     &typeCode,
	                     &dataSize);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&l",
	                     PyMac_BuildOSType, typeCode,
	                     dataSize);
	return _res;
}

static PyObject *AEDesc_AEDeleteParam(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
#ifndef AEDeleteParam
	PyMac_PRECHECK(AEDeleteParam);
#endif
	if (!PyArg_ParseTuple(_args, "O&",
	                      PyMac_GetOSType, &theAEKeyword))
		return NULL;
	_err = AEDeleteParam(&_self->ob_itself,
	                     theAEKeyword);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AEGetAttributePtr(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
	DescType desiredType;
	DescType typeCode;
	char *dataPtr__out__;
	long dataPtr__len__;
	int dataPtr__in_len__; // TO DO: add dataPtr__actual_len__, dataPtr__out_len__; dataPtr__out_len__ = (dataPtr__actual_len__ > dataPtr__in_len__) ? dataPtr__in_len__ : dataPtr__actual_len__; use dataPtr__out_len__ to specify buffer size in Py_BuildValue, then return dataPtr__actual_len__ so clients can compare this against the value they supplied for dataPtr__in_len__ and decide if they want to try again with a larger buffer
#ifndef AEGetAttributePtr
	PyMac_PRECHECK(AEGetAttributePtr);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&i",
	                      PyMac_GetOSType, &theAEKeyword,
	                      PyMac_GetOSType, &desiredType,
	                      &dataPtr__in_len__)) // TO DO: note: client will normally use AESizeOfAttribute to obtain this value in advance, unless they specify a desiredType different to actual type
		return NULL;
	if ((dataPtr__out__ = malloc(dataPtr__in_len__)) == NULL)
	{
		PyErr_NoMemory();
		goto dataPtr__error__;
	}
	dataPtr__len__ = dataPtr__in_len__;
	_err = AEGetAttributePtr(&_self->ob_itself,
	                         theAEKeyword,
	                         desiredType,
	                         &typeCode,
	                         dataPtr__out__, dataPtr__len__, &dataPtr__len__); // TO DO: will cause Py_BuildValue to overrun buffer if caller's buffer size is too small; if actual size > buffer size, need to resize buffer and call again; simplest is to return actual size to caller and let them decide
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&s#",
	                     PyMac_BuildOSType, typeCode,
	                     dataPtr__out__, (int)dataPtr__len__);
	free(dataPtr__out__);
 dataPtr__error__: ;
	return _res;
}

static PyObject *AEDesc_AEGetAttributeDesc(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
	DescType desiredType;
	AEDesc result;
#ifndef AEGetAttributeDesc
	PyMac_PRECHECK(AEGetAttributeDesc);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&",
	                      PyMac_GetOSType, &theAEKeyword,
	                      PyMac_GetOSType, &desiredType))
		return NULL;
	_err = AEGetAttributeDesc(&_self->ob_itself,
	                          theAEKeyword,
	                          desiredType,
	                          &result);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &result);
	return _res;
}

static PyObject *AEDesc_AESizeOfAttribute(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
	DescType typeCode;
	Size dataSize;
#ifndef AESizeOfAttribute
	PyMac_PRECHECK(AESizeOfAttribute);
#endif
	if (!PyArg_ParseTuple(_args, "O&",
	                      PyMac_GetOSType, &theAEKeyword))
		return NULL;
	_err = AESizeOfAttribute(&_self->ob_itself,
	                         theAEKeyword,
	                         &typeCode,
	                         &dataSize);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&l",
	                     PyMac_BuildOSType, typeCode,
	                     dataSize);
	return _res;
}

static PyObject *AEDesc_AEPutAttributePtr(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
	DescType typeCode;
	char *dataPtr__in__;
	long dataPtr__len__;
	int dataPtr__in_len__;
#ifndef AEPutAttributePtr
	PyMac_PRECHECK(AEPutAttributePtr);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&s#",
	                      PyMac_GetOSType, &theAEKeyword,
	                      PyMac_GetOSType, &typeCode,
	                      &dataPtr__in__, &dataPtr__in_len__))
		return NULL;
	dataPtr__len__ = dataPtr__in_len__;
	_err = AEPutAttributePtr(&_self->ob_itself,
	                         theAEKeyword,
	                         typeCode,
	                         dataPtr__in__, dataPtr__len__);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AEPutAttributeDesc(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword theAEKeyword;
	AEDesc theAEDesc;
#ifndef AEPutAttributeDesc
	PyMac_PRECHECK(AEPutAttributeDesc);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&",
	                      PyMac_GetOSType, &theAEKeyword,
	                      AEDescX_Convert, &theAEDesc))
		return NULL;
	_err = AEPutAttributeDesc(&_self->ob_itself,
	                          theAEKeyword,
	                          &theAEDesc);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AEGetDescDataSize(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	Size _rv;
#ifndef AEGetDescDataSize
	PyMac_PRECHECK(AEGetDescDataSize);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	_rv = AEGetDescDataSize(&_self->ob_itself);
	_res = Py_BuildValue("l",
	                     _rv);
	return _res;
}

static PyObject *AEDesc_AESend(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AppleEvent reply;
	AESendMode sendMode;
	AESendPriority sendPriority;
	long timeOutInTicks;
#ifndef AESend
	PyMac_PRECHECK(AESend);
#endif
	if (!PyArg_ParseTuple(_args, "lhl",
	                      &sendMode,
	                      &sendPriority,
	                      &timeOutInTicks))
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	_err = AESend(&_self->ob_itself,
	              &reply,
	              sendMode,
	              sendPriority,
	              timeOutInTicks,
	              upp_AEIdleProc,
	              (AEFilterUPP)0);
	Py_END_ALLOW_THREADS
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &reply);
	return _res;
}


static PyObject *AEDesc_AESendMessageAllocatingMachPort(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AppleEvent reply;
	AESendMode sendMode;
	long timeOutInTicks;
	mach_port_t replyPort;
#ifndef AESend
	PyMac_PRECHECK(AESend);
#endif
	if (!PyArg_ParseTuple(_args, "ll",
						  &sendMode,
						  &timeOutInTicks))
		return NULL;
	if (sendMode == kAEWaitReply) {
		if (mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &replyPort) != KERN_SUCCESS) {
			PyErr_SetString(PyExc_SystemError, "Failed to allocate Mach port!");
			return NULL;
		}
		_err = AEPutAttributePtr(&_self->ob_itself,
								 keyReplyPortAttr,
								 typeMachPort,
								 &replyPort,
								 sizeof(replyPort));
		if (_err != noErr) goto cleanup;
	}
	_err = AESendMessage(&_self->ob_itself,
						 &reply,
						 sendMode,
						 timeOutInTicks);
	if (_err != noErr) goto cleanup;
	_res = Py_BuildValue("O&",
						 AEDescX_New, &reply);
	if (sendMode == kAEWaitReply) {
		mach_port_destroy(mach_task_self(), replyPort);
	}
	return _res;
cleanup:
	if (sendMode == kAEWaitReply) {
		mach_port_destroy(mach_task_self(), replyPort);
	}
	return PyMac_Error(_err);
}



static PyObject *AEDesc_AESendMessage(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AppleEvent reply;
	AESendMode sendMode;
	long timeOutInTicks;
#ifndef AESendMessage
	PyMac_PRECHECK(AESendMessage);
#endif
	if (!PyArg_ParseTuple(_args, "ll",
						  &sendMode,
						  &timeOutInTicks))
		return NULL;
	_err = AESendMessage(&_self->ob_itself,
						 &reply,
						 sendMode,
						 timeOutInTicks);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
						 AEDescX_New, &reply);
	return _res;
}

static PyObject *AEDesc_AEResetTimer(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
#ifndef AEResetTimer
	PyMac_PRECHECK(AEResetTimer);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	_err = AEResetTimer(&_self->ob_itself);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AESuspendTheCurrentEvent(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
#ifndef AESuspendTheCurrentEvent
	PyMac_PRECHECK(AESuspendTheCurrentEvent);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	_err = AESuspendTheCurrentEvent(&_self->ob_itself);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AEResumeTheCurrentEvent(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AppleEvent reply;
	AEEventHandlerUPP dispatcher__proc__ = upp_GenericEventHandler;
	PyObject *dispatcher;
#ifndef AEResumeTheCurrentEvent
	PyMac_PRECHECK(AEResumeTheCurrentEvent);
#endif
	if (!PyArg_ParseTuple(_args, "O&O",
	                      AEDescX_Convert, &reply,
	                      &dispatcher))
		return NULL;
	_err = AEResumeTheCurrentEvent(&_self->ob_itself,
	                               &reply,
	                               dispatcher__proc__, (long)dispatcher);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	Py_INCREF(dispatcher); /* XXX leak, but needed */
	return _res;
}

static PyObject *AEDesc_AEGetTheCurrentEvent(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
#ifndef AEGetTheCurrentEvent
	PyMac_PRECHECK(AEGetTheCurrentEvent);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	_err = AEGetTheCurrentEvent(&_self->ob_itself);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AESetTheCurrentEvent(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
#ifndef AESetTheCurrentEvent
	PyMac_PRECHECK(AESetTheCurrentEvent);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	_err = AESetTheCurrentEvent(&_self->ob_itself);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AEDesc_AEResolve(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	short callbackFlags;
	AEDesc theToken;
#ifndef AEResolve
	PyMac_PRECHECK(AEResolve);
#endif
	if (!PyArg_ParseTuple(_args, "h",
	                      &callbackFlags))
		return NULL;
	_err = AEResolve(&_self->ob_itself,
	                 callbackFlags,
	                 &theToken);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &theToken);
	return _res;
}

static PyObject *AEDesc_AutoDispose(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;

	int onoff, old;
	if (!PyArg_ParseTuple(_args, "i", &onoff))
	        return NULL;
	old = _self->ob_owned;
	_self->ob_owned = onoff;
	_res = Py_BuildValue("i", old);
	return _res;
}

static PyObject *AEDesc_AEReplaceDescData(AEDescObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	DescType typeCode;
	char *dataPtr__in__;
	long dataPtr__len__;
	int dataPtr__in_len__;
#ifndef AEReplaceDescData
	PyMac_PRECHECK(AEReplaceDescData);
#endif
	if (!PyArg_ParseTuple(_args, "O&s#",
	                      PyMac_GetOSType, &typeCode,
	                      &dataPtr__in__, &dataPtr__in_len__))
		return NULL;
	dataPtr__len__ = dataPtr__in_len__;
	_err = AEReplaceDescData(typeCode,
	                         dataPtr__in__, dataPtr__len__,
	                         &_self->ob_itself);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_None;
	return _res;
}

static PyMethodDef AEDesc_methods[] = {
	{"AECoerceDesc", (PyCFunction)AEDesc_AECoerceDesc, 1,
	 PyDoc_STR("(DescType toType) -> (AEDesc result)")},
	{"AEDuplicateDesc", (PyCFunction)AEDesc_AEDuplicateDesc, 1,
	 PyDoc_STR("() -> (AEDesc result)")},
	{"AECountItems", (PyCFunction)AEDesc_AECountItems, 1,
	 PyDoc_STR("() -> (long theCount)")},
	{"AECheckIsRecord", (PyCFunction)AEDesc_AECheckIsRecord, 1,
	 PyDoc_STR("() -> (Boolean isRecord)")},
	{"AEPutPtr", (PyCFunction)AEDesc_AEPutPtr, 1,
	 PyDoc_STR("(long index, DescType typeCode, Buffer dataPtr) -> None")},
	{"AEPutDesc", (PyCFunction)AEDesc_AEPutDesc, 1,
	 PyDoc_STR("(long index, AEDesc theAEDesc) -> None")},
	{"AEGetNthPtr", (PyCFunction)AEDesc_AEGetNthPtr, 1,
	 PyDoc_STR("(long index, DescType desiredType, Buffer dataPtr) -> (AEKeyword theAEKeyword, DescType typeCode, Buffer dataPtr)")},
	{"AEGetNthDesc", (PyCFunction)AEDesc_AEGetNthDesc, 1,
	 PyDoc_STR("(long index, DescType desiredType) -> (AEKeyword theAEKeyword, AEDesc result)")},
	{"AESizeOfNthItem", (PyCFunction)AEDesc_AESizeOfNthItem, 1,
	 PyDoc_STR("(long index) -> (DescType typeCode, Size dataSize)")},
	{"AEDeleteItem", (PyCFunction)AEDesc_AEDeleteItem, 1,
	 PyDoc_STR("(long index) -> None")},
	{"AEPutParamPtr", (PyCFunction)AEDesc_AEPutParamPtr, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword, DescType typeCode, Buffer dataPtr) -> None")},
	{"AEPutParamDesc", (PyCFunction)AEDesc_AEPutParamDesc, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword, AEDesc theAEDesc) -> None")},
	{"AEGetParamPtr", (PyCFunction)AEDesc_AEGetParamPtr, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword, DescType desiredType, Buffer dataPtr) -> (DescType typeCode, Buffer dataPtr)")},
	{"AEGetParamDesc", (PyCFunction)AEDesc_AEGetParamDesc, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword, DescType desiredType) -> (AEDesc result)")},
	{"AESizeOfParam", (PyCFunction)AEDesc_AESizeOfParam, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword) -> (DescType typeCode, Size dataSize)")},
	{"AEDeleteParam", (PyCFunction)AEDesc_AEDeleteParam, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword) -> None")},
	{"AEGetAttributePtr", (PyCFunction)AEDesc_AEGetAttributePtr, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword, DescType desiredType, Buffer dataPtr) -> (DescType typeCode, Buffer dataPtr)")},
	{"AEGetAttributeDesc", (PyCFunction)AEDesc_AEGetAttributeDesc, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword, DescType desiredType) -> (AEDesc result)")},
	{"AESizeOfAttribute", (PyCFunction)AEDesc_AESizeOfAttribute, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword) -> (DescType typeCode, Size dataSize)")},
	{"AEPutAttributePtr", (PyCFunction)AEDesc_AEPutAttributePtr, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword, DescType typeCode, Buffer dataPtr) -> None")},
	{"AEPutAttributeDesc", (PyCFunction)AEDesc_AEPutAttributeDesc, 1,
	 PyDoc_STR("(AEKeyword theAEKeyword, AEDesc theAEDesc) -> None")},
	{"AEGetDescDataSize", (PyCFunction)AEDesc_AEGetDescDataSize, 1,
	 PyDoc_STR("() -> (Size _rv)")},
	{"AESend", (PyCFunction)AEDesc_AESend, 1,
	 PyDoc_STR("(AESendMode sendMode, AESendPriority sendPriority, long timeOutInTicks) -> (AppleEvent reply)")},
	{"AESendMessage", (PyCFunction)AEDesc_AESendMessage, 1,
	 PyDoc_STR("(AESendMode sendMode, long timeOutInTicks) -> (AppleEvent reply)")}, 
	{"AESendMessageAllocatingMachPort", (PyCFunction)AEDesc_AESendMessageAllocatingMachPort, 1,
	 PyDoc_STR("(AESendMode sendMode, long timeOutInTicks) -> (AppleEvent reply)")},
	{"AEResetTimer", (PyCFunction)AEDesc_AEResetTimer, 1,
	 PyDoc_STR("() -> None")},
	{"AESuspendTheCurrentEvent", (PyCFunction)AEDesc_AESuspendTheCurrentEvent, 1,
	 PyDoc_STR("() -> None")},
	{"AEResumeTheCurrentEvent", (PyCFunction)AEDesc_AEResumeTheCurrentEvent, 1,
	 PyDoc_STR("(AppleEvent reply, EventHandler dispatcher) -> None")},
	{"AEGetTheCurrentEvent", (PyCFunction)AEDesc_AEGetTheCurrentEvent, 1,
	 PyDoc_STR("() -> None")},
	{"AESetTheCurrentEvent", (PyCFunction)AEDesc_AESetTheCurrentEvent, 1,
	 PyDoc_STR("() -> None")},
	{"AEResolve", (PyCFunction)AEDesc_AEResolve, 1,
	 PyDoc_STR("(short callbackFlags) -> (AEDesc theToken)")},
	{"AutoDispose", (PyCFunction)AEDesc_AutoDispose, 1,
	 PyDoc_STR("(int)->int. Automatically AEDisposeDesc the object on Python object cleanup")},
	{"AEReplaceDescData", (PyCFunction)AEDesc_AEReplaceDescData, 1,
	 PyDoc_STR("(DescType typeCode, Buffer dataPtr) -> None")},
	{NULL, NULL, 0}
};

static PyObject *AEDesc_get_type(AEDescObject *self, void *closure)
{
	return PyMac_BuildOSType(self->ob_itself.descriptorType);
}

#define AEDesc_set_type NULL

static PyObject *AEDesc_get_data(AEDescObject *self, void *closure)
{

			PyObject *res;
			Size size;
			char *ptr;
			OSErr err;
			
			size = AEGetDescDataSize(&self->ob_itself);
			if ( (res = PyString_FromStringAndSize(NULL, size)) == NULL )
				return NULL;
			if ( (ptr = PyString_AsString(res)) == NULL )
				return NULL;
			if ( (err=AEGetDescData(&self->ob_itself, ptr, size)) < 0 )
				return PyMac_Error(err);	
			return res;
			
}

#define AEDesc_set_data NULL

static PyGetSetDef AEDesc_getsetlist[] = {
	{"type", (getter)AEDesc_get_type, (setter)AEDesc_set_type, "Type of this AEDesc"},
	{"data", (getter)AEDesc_get_data, (setter)AEDesc_set_data, "The raw data in this AEDesc"},
	{NULL, NULL, NULL, NULL},
};


#define AEDesc_compare NULL

#define AEDesc_repr NULL

#define AEDesc_hash NULL
#define AEDesc_tp_init 0

#define AEDesc_tp_alloc PyType_GenericAlloc

static PyObject *AEDesc_tp_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PyObject *self;
	AEDesc itself;
	char *kw[] = {"itself", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&", kw, AEDescX_Convert, &itself)) return NULL;
	if ((self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((AEDescObject *)self)->ob_itself = itself;
	return self;
}

#define AEDesc_tp_free PyObject_Del


PyTypeObject AEDesc_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"CarbonX._AE.AEDesc", /*tp_name*/
	sizeof(AEDescObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) AEDesc_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) AEDesc_compare, /*tp_compare*/
	(reprfunc) AEDesc_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) AEDesc_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	AEDesc_methods, /* tp_methods */
	0, /*tp_members*/
	AEDesc_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	AEDesc_tp_init, /* tp_init */
	AEDesc_tp_alloc, /* tp_alloc */
	AEDesc_tp_new, /* tp_new */
	AEDesc_tp_free, /* tp_free */
};

/* --------------------- End object type AEDesc --------------------- */


static PyObject *AE_AECoercePtr(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	DescType typeCode;
	char *dataPtr__in__;
	long dataPtr__len__;
	int dataPtr__in_len__;
	DescType toType;
	AEDesc result;
#ifndef AECoercePtr
	PyMac_PRECHECK(AECoercePtr);
#endif
	if (!PyArg_ParseTuple(_args, "O&s#O&",
	                      PyMac_GetOSType, &typeCode,
	                      &dataPtr__in__, &dataPtr__in_len__,
	                      PyMac_GetOSType, &toType))
		return NULL;
	dataPtr__len__ = dataPtr__in_len__;
	_err = AECoercePtr(typeCode,
	                   dataPtr__in__, dataPtr__len__,
	                   toType,
	                   &result);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &result);
	return _res;
}

static PyObject *AE_AECreateDesc(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	DescType typeCode;
	char *dataPtr__in__;
	long dataPtr__len__;
	int dataPtr__in_len__;
	AEDesc result;
#ifndef AECreateDesc
	PyMac_PRECHECK(AECreateDesc);
#endif
	if (!PyArg_ParseTuple(_args, "O&s#",
	                      PyMac_GetOSType, &typeCode,
	                      &dataPtr__in__, &dataPtr__in_len__))
		return NULL;
	dataPtr__len__ = dataPtr__in_len__;
	_err = AECreateDesc(typeCode,
	                    dataPtr__in__, dataPtr__len__,
	                    &result);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &result);
	return _res;
}

static PyObject *AE_AECreateList(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	char *factoringPtr__in__;
	long factoringPtr__len__;
	int factoringPtr__in_len__;
	Boolean isRecord;
	AEDescList resultList;
#ifndef AECreateList
	PyMac_PRECHECK(AECreateList);
#endif
	if (!PyArg_ParseTuple(_args, "s#b",
	                      &factoringPtr__in__, &factoringPtr__in_len__,
	                      &isRecord))
		return NULL;
	factoringPtr__len__ = factoringPtr__in_len__;
	_err = AECreateList(factoringPtr__in__, factoringPtr__len__,
	                    isRecord,
	                    &resultList);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &resultList);
	return _res;
}

static PyObject *AE_AECreateAppleEvent(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEEventClass theAEEventClass;
	AEEventID theAEEventID;
	AEAddressDesc target;
	AEReturnID returnID;
	AETransactionID transactionID;
	AppleEvent result;
#ifndef AECreateAppleEvent
	PyMac_PRECHECK(AECreateAppleEvent);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&O&hl",
	                      PyMac_GetOSType, &theAEEventClass,
	                      PyMac_GetOSType, &theAEEventID,
	                      AEDescX_Convert, &target,
	                      &returnID,
	                      &transactionID))
		return NULL;
	_err = AECreateAppleEvent(theAEEventClass,
	                          theAEEventID,
	                          &target,
	                          returnID,
	                          transactionID,
	                          &result);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &result);
	return _res;
}

static PyObject *AE_AEProcessAppleEvent(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	EventRecord theEventRecord;
#ifndef AEProcessAppleEvent
	PyMac_PRECHECK(AEProcessAppleEvent);
#endif
	if (!PyArg_ParseTuple(_args, "O&",
	                      PyMac_GetEventRecord, &theEventRecord))
		return NULL;
	_err = AEProcessAppleEvent(&theEventRecord);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AE_AEGetInteractionAllowed(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEInteractAllowed level;
#ifndef AEGetInteractionAllowed
	PyMac_PRECHECK(AEGetInteractionAllowed);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	_err = AEGetInteractionAllowed(&level);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("b",
	                     level);
	return _res;
}

static PyObject *AE_AESetInteractionAllowed(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEInteractAllowed level;
#ifndef AESetInteractionAllowed
	PyMac_PRECHECK(AESetInteractionAllowed);
#endif
	if (!PyArg_ParseTuple(_args, "b",
	                      &level))
		return NULL;
	_err = AESetInteractionAllowed(level);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AE_AEInteractWithUser(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	long timeOutInTicks;
#ifndef AEInteractWithUser
	PyMac_PRECHECK(AEInteractWithUser);
#endif
	if (!PyArg_ParseTuple(_args, "l",
	                      &timeOutInTicks))
		return NULL;
	_err = AEInteractWithUser(timeOutInTicks,
	                          (NMRecPtr)0,
	                          upp_AEIdleProc);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AE_AEInstallEventHandler(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEEventClass theAEEventClass;
	AEEventID theAEEventID;
	AEEventHandlerUPP handler__proc__ = upp_GenericEventHandler;
	PyObject *handler;
#ifndef AEInstallEventHandler
	PyMac_PRECHECK(AEInstallEventHandler);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&O",
	                      PyMac_GetOSType, &theAEEventClass,
	                      PyMac_GetOSType, &theAEEventID,
	                      &handler))
		return NULL;
	_err = AEInstallEventHandler(theAEEventClass,
	                             theAEEventID,
	                             handler__proc__, (long)handler,
	                             0);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	Py_INCREF(handler); /* XXX leak, but needed */
	return _res;
}

static PyObject *AE_AERemoveEventHandler(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEEventClass theAEEventClass;
	AEEventID theAEEventID;
#ifndef AERemoveEventHandler
	PyMac_PRECHECK(AERemoveEventHandler);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&",
	                      PyMac_GetOSType, &theAEEventClass,
	                      PyMac_GetOSType, &theAEEventID))
		return NULL;
	_err = AERemoveEventHandler(theAEEventClass,
	                            theAEEventID,
	                            upp_GenericEventHandler,
	                            0);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AE_AEGetEventHandler(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEEventClass theAEEventClass;
	AEEventID theAEEventID;
	AEEventHandlerUPP handler__proc__ = upp_GenericEventHandler;
	PyObject *handler;
#ifndef AEGetEventHandler
	PyMac_PRECHECK(AEGetEventHandler);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&",
	                      PyMac_GetOSType, &theAEEventClass,
	                      PyMac_GetOSType, &theAEEventID))
		return NULL;
	_err = AEGetEventHandler(theAEEventClass,
	                         theAEEventID,
	                         &handler__proc__, (long *)&handler,
	                         0);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O",
	                     handler);
	Py_INCREF(handler); /* XXX leak, but needed */
	return _res;
}

static PyObject *AE_AEInstallCoercionHandler(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	DescType fromType, toType;
	AECoercionHandlerUPP handler__proc__ = upp_GenericCoercionHandler;
	PyObject *handler;
#ifndef AEInstallCoercionHandler
	PyMac_PRECHECK(AEInstallCoercionHandler);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&O",
	                      PyMac_GetOSType, &fromType,
	                      PyMac_GetOSType, &toType,
	                      &handler))
		return NULL;
	_err = AEInstallCoercionHandler(fromType,
	                                toType,
	                                handler__proc__, (long)handler,
	                                1, 0);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	Py_INCREF(handler); /* XXX leak, but needed */
	return _res;
}

static PyObject *AE_AERemoveCoercionHandler(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	DescType fromType, toType;
#ifndef AERemoveCoercionHandler
	PyMac_PRECHECK(AERemoveCoercionHandler);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&",
	                      PyMac_GetOSType, &fromType,
	                      PyMac_GetOSType, &toType))
		return NULL;
	_err = AERemoveCoercionHandler(fromType,
	                            toType,
	                            upp_GenericCoercionHandler,
	                            0);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AE_AEGetCoercionHandler(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	DescType fromType, toType;
	AECoercionHandlerUPP handler__proc__ = upp_GenericCoercionHandler;
	PyObject *handler;
	Boolean fromTypeIsDesc;
#ifndef AEGetCoercionHandler
	PyMac_PRECHECK(AEGetCoercionHandler);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&",
	                      PyMac_GetOSType, &fromType,
	                      PyMac_GetOSType, &toType))
		return NULL;
	_err = AEGetCoercionHandler(fromType,
	                         toType,
	                         &handler__proc__, (long *)&handler,
	                         &fromTypeIsDesc,
	                         0);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("Ob",
	                     handler,
	                     fromTypeIsDesc);
	Py_INCREF(handler); /* XXX leak, but needed */
	return _res;
}

static PyObject *AE_AEInstallSpecialHandler(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword functionClass;
#ifndef AEInstallSpecialHandler
	PyMac_PRECHECK(AEInstallSpecialHandler);
#endif
	if (!PyArg_ParseTuple(_args, "O&",
	                      PyMac_GetOSType, &functionClass))
		return NULL;
	_err = AEInstallSpecialHandler(functionClass,
	                               upp_GenericEventHandler,
	                               0);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AE_AERemoveSpecialHandler(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword functionClass;
#ifndef AERemoveSpecialHandler
	PyMac_PRECHECK(AERemoveSpecialHandler);
#endif
	if (!PyArg_ParseTuple(_args, "O&",
	                      PyMac_GetOSType, &functionClass))
		return NULL;
	_err = AERemoveSpecialHandler(functionClass,
	                              upp_GenericEventHandler,
	                              0);
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AE_AEManagerInfo(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEKeyword keyWord;
	long result;
#ifndef AEManagerInfo
	PyMac_PRECHECK(AEManagerInfo);
#endif
	if (!PyArg_ParseTuple(_args, "O&",
	                      PyMac_GetOSType, &keyWord))
		return NULL;
	_err = AEManagerInfo(keyWord,
	                     &result);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("l",
	                     result);
	return _res;
}

static PyObject *AE_AEObjectInit(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
#ifndef AEObjectInit
	PyMac_PRECHECK(AEObjectInit);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	_err = AEObjectInit();
	if (_err != noErr) return PyMac_Error(_err);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AE_AEDisposeToken(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	AEDesc theToken;
#ifndef AEDisposeToken
	PyMac_PRECHECK(AEDisposeToken);
#endif
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	_err = AEDisposeToken(&theToken);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &theToken);
	return _res;
}

static PyObject *AE_AECallObjectAccessor(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	OSErr _err;
	DescType desiredClass;
	AEDesc containerToken;
	DescType containerClass;
	DescType keyForm;
	AEDesc keyData;
	AEDesc token;
#ifndef AECallObjectAccessor
	PyMac_PRECHECK(AECallObjectAccessor);
#endif
	if (!PyArg_ParseTuple(_args, "O&O&O&O&O&",
	                      PyMac_GetOSType, &desiredClass,
	                      AEDescX_Convert, &containerToken,
	                      PyMac_GetOSType, &containerClass,
	                      PyMac_GetOSType, &keyForm,
	                      AEDescX_Convert, &keyData))
		return NULL;
	_err = AECallObjectAccessor(desiredClass,
	                            &containerToken,
	                            containerClass,
	                            keyForm,
	                            &keyData,
	                            &token);
	if (_err != noErr) return PyMac_Error(_err);
	_res = Py_BuildValue("O&",
	                     AEDescX_New, &token);
	return _res;
}

static PyMethodDef AE_methods[] = {
	{"AECoercePtr", (PyCFunction)AE_AECoercePtr, 1,
	 PyDoc_STR("(DescType typeCode, Buffer dataPtr, DescType toType) -> (AEDesc result)")},
	{"AECreateDesc", (PyCFunction)AE_AECreateDesc, 1,
	 PyDoc_STR("(DescType typeCode, Buffer dataPtr) -> (AEDesc result)")},
	{"AECreateList", (PyCFunction)AE_AECreateList, 1,
	 PyDoc_STR("(Buffer factoringPtr, Boolean isRecord) -> (AEDescList resultList)")},
	{"AECreateAppleEvent", (PyCFunction)AE_AECreateAppleEvent, 1,
	 PyDoc_STR("(AEEventClass theAEEventClass, AEEventID theAEEventID, AEAddressDesc target, AEReturnID returnID, AETransactionID transactionID) -> (AppleEvent result)")},
	{"AEProcessAppleEvent", (PyCFunction)AE_AEProcessAppleEvent, 1,
	 PyDoc_STR("(EventRecord theEventRecord) -> None")},
	{"AEGetInteractionAllowed", (PyCFunction)AE_AEGetInteractionAllowed, 1,
	 PyDoc_STR("() -> (AEInteractAllowed level)")},
	{"AESetInteractionAllowed", (PyCFunction)AE_AESetInteractionAllowed, 1,
	 PyDoc_STR("(AEInteractAllowed level) -> None")},
	{"AEInteractWithUser", (PyCFunction)AE_AEInteractWithUser, 1,
	 PyDoc_STR("(long timeOutInTicks) -> None")},
	{"AEInstallEventHandler", (PyCFunction)AE_AEInstallEventHandler, 1,
	 PyDoc_STR("(AEEventClass theAEEventClass, AEEventID theAEEventID, EventHandler handler) -> None")},
	{"AERemoveEventHandler", (PyCFunction)AE_AERemoveEventHandler, 1,
	 PyDoc_STR("(AEEventClass theAEEventClass, AEEventID theAEEventID) -> None")},
	{"AEGetEventHandler", (PyCFunction)AE_AEGetEventHandler, 1,
	 PyDoc_STR("(AEEventClass theAEEventClass, AEEventID theAEEventID) -> (EventHandler handler)")},
	{"AEInstallCoercionHandler", (PyCFunction)AE_AEInstallCoercionHandler, 1,
	 PyDoc_STR("(DescType fromType, DescType toType, CoercionHandler handler) -> None")},
	{"AERemoveCoercionHandler", (PyCFunction)AE_AERemoveCoercionHandler, 1,
	 PyDoc_STR("(DescType fromType, DescType toType) -> None")},
	{"AEGetCoercionHandler", (PyCFunction)AE_AEGetCoercionHandler, 1,
	 PyDoc_STR("(DescType fromType, DescType toType) -> (CoercionHandler handler, Boolean fromTypeIsDesc)")},
	{"AEInstallSpecialHandler", (PyCFunction)AE_AEInstallSpecialHandler, 1,
	 PyDoc_STR("(AEKeyword functionClass) -> None")},
	{"AERemoveSpecialHandler", (PyCFunction)AE_AERemoveSpecialHandler, 1,
	 PyDoc_STR("(AEKeyword functionClass) -> None")},
	{"AEManagerInfo", (PyCFunction)AE_AEManagerInfo, 1,
	 PyDoc_STR("(AEKeyword keyWord) -> (long result)")},
	{"AEObjectInit", (PyCFunction)AE_AEObjectInit, 1,
	 PyDoc_STR("() -> None")},
	{"AEDisposeToken", (PyCFunction)AE_AEDisposeToken, 1,
	 PyDoc_STR("() -> (AEDesc theToken)")},
	{"AECallObjectAccessor", (PyCFunction)AE_AECallObjectAccessor, 1,
	 PyDoc_STR("(DescType desiredClass, AEDesc containerToken, DescType containerClass, DescType keyForm, AEDesc keyData) -> (AEDesc token)")},
	{NULL, NULL, 0}
};



static pascal OSErr
GenericEventHandler(const AppleEvent *request, AppleEvent *reply, refcontype refcon)
{
	PyObject *handler = (PyObject *)refcon;
	AEDescObject *requestObject, *replyObject;
	PyObject *args, *res;
    PyGILState_STATE state;
	OSErr err = noErr;
	
	state = PyGILState_Ensure();
	if ((requestObject = (AEDescObject *)AEDescX_New((AppleEvent *)request)) == NULL) {
		err = -1;
		goto cleanup;
	}
	if ((replyObject = (AEDescObject *)AEDescX_New(reply)) == NULL) {
		Py_DECREF(requestObject);
		err = -1;
		goto cleanup;
	}
	if ((args = Py_BuildValue("OO", requestObject, replyObject)) == NULL) {
		Py_DECREF(requestObject);
		Py_DECREF(replyObject);
		err = -1;
		goto cleanup;
	}
	res = PyEval_CallObject(handler, args);
	requestObject->ob_itself.descriptorType = 'null';
	requestObject->ob_itself.dataHandle = NULL;
	replyObject->ob_itself.descriptorType = 'null';
	replyObject->ob_itself.dataHandle = NULL;
	Py_DECREF(args);
	if (res == NULL) {
		PySys_WriteStderr("Exception in AE event handler function\n");
		PyErr_Print();
		err = -1;
		goto cleanup;
	}
	Py_DECREF(res);
cleanup:
	PyGILState_Release(state);
	return err;
}

static pascal OSErr
GenericCoercionHandler(const AEDesc *fromDesc, DescType toType, refcontype refcon, AEDesc *toDesc)
{	
	PyObject *handler = (PyObject *)refcon;
	AEDescObject *fromObject;
	PyObject *args, *res;
    PyGILState_STATE state;
	OSErr err = noErr;
	
	state = PyGILState_Ensure();
	if ((fromObject = (AEDescObject *)AEDescX_New((AEDesc *)fromDesc)) == NULL) {
		err = -1;
		goto cleanup;
	}
	if ((args = Py_BuildValue("OO&", fromObject, PyMac_BuildOSType, &toType)) == NULL) {
		Py_DECREF(fromObject);
		err = -1;
		goto cleanup;
	}
	res = PyEval_CallObject(handler, args);
	fromObject->ob_itself.descriptorType = 'null';
	fromObject->ob_itself.dataHandle = NULL;
	Py_DECREF(args);
	if (res == NULL) {
		PySys_WriteStderr("Exception in AE coercion handler function\n");
		PyErr_Print();
		err = errAECoercionFail;
		goto cleanup;
	}
	if (!AEDesc_Check(res)) {
		PySys_WriteStderr("AE coercion handler function did not return an AEDesc\n");
		Py_DECREF(res);
		err = errAECoercionFail;
		goto cleanup;
	}
	if (AEDuplicateDesc(&((AEDescObject *)res)->ob_itself, toDesc)) {
		Py_DECREF(res);
		err = -1;
		goto cleanup;
	}
	Py_DECREF(res);
cleanup:
	PyGILState_Release(state);
	return err;
}

PyObject *AEDescX_NewBorrowed(AEDesc *itself)
{
	PyObject *it;
	
	it = AEDescX_New(itself);
	if (it)
		((AEDescObject *)it)->ob_owned = 0;
	return (PyObject *)it;
}

int AEDescX_ConvertDisown(PyObject *v, AEDesc *p_itself) {
	if (!AEDescX_Convert(v, p_itself)) return 0;
	((AEDescObject *)v)->ob_owned = 0;
	return 1;
}

CarbonXAE_API aeAPI = {
	AEDescX_New,
	AEDescX_NewBorrowed,
	AEDescX_Convert,
	AEDescX_ConvertDisown
};


void init_AE(void)
{
	PyObject *m;
	PyObject *d;



		upp_AEIdleProc = NewAEIdleUPP(AEIdleProc);
		upp_GenericEventHandler = NewAEEventHandlerUPP(GenericEventHandler);
		upp_GenericCoercionHandler = NewAECoerceDescUPP(GenericCoercionHandler);

	

	m = Py_InitModule("_AE", AE_methods);
	d = PyModule_GetDict(m);
	AE_Error = PyMac_GetOSErrException();
	if (AE_Error == NULL ||
	    PyDict_SetItemString(d, "Error", AE_Error) != 0)
		return;
	AEDesc_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&AEDesc_Type) < 0) return;
	Py_INCREF(&AEDesc_Type);
	PyModule_AddObject(m, "AEDesc", (PyObject *)&AEDesc_Type);
	/* Backward-compatible name */
	Py_INCREF(&AEDesc_Type);
	PyModule_AddObject(m, "AEDescType", (PyObject *)&AEDesc_Type);

	PyObject *aeAPIObj = PyCObject_FromVoidPtr((void *)&aeAPI, NULL);
	PyModule_AddObject(m, "aeAPI", aeAPIObj);
}

/* ========================= End module _AE ========================= */

