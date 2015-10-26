/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <Core/Config.h>

#ifndef FEC_SHARED
  #define FEC_SHARED
#endif
#include <Core/Clients/CAPI/FabricCore.h>

#include <Python.h>
#include <stddef.h>

#include <assert.h>
#include <map>
#if defined( FABRIC_OS_WINDOWS )
# include <malloc.h>
#else
# include <alloca.h>
#endif

#include "PythonCAPI.h"

FabricCore::RTValCodecResult PythonRTValToJSONEncoder::encodeRTValToJSON(
  FabricCore::Context const &context,
  FabricCore::RTVal const &rtVal,
  FabricCore::RTValCodecContext codecContext,
  AppendFunctor const &append,
  MetadataLookupFunctor const &metadataLookup
  )
{
  PyObject *pyRTVal = RTValToPyObject( &context, &rtVal );

  PyGILState_STATE gstate = PyGILState_Ensure();

  PyObject *pyArgs = PyTuple_New( 1 );
  PyTuple_SetItem( pyArgs, 0, pyRTVal );
  PyObject *pyResult = PyEval_CallObject( m_pyRTValToJSONEncoder, pyArgs );
  Py_DECREF( pyArgs );

  FabricCore::RTValCodecResult result =
    FabricCore::RTValCodecResult_Reject;

  bool isNone = pyResult == Py_None;
  bool isTrue = pyResult == Py_True;
  bool isFalse = pyResult == Py_False;
  bool isString = PyString_Check( pyResult ) || PyUnicode_Check( pyResult );

  if ( !isNone && !isTrue && !isFalse && !isString )
  {
    PyErr_SetString(
      PyExc_TypeError,
      "rtValToJSONEncoder must return a string, True, False or None"
      );
  }
  else if ( isString )
  {
    char const *cStrResult = PyString_AsString( pyResult );
    if ( cStrResult && *cStrResult )
    {
      result = FabricCore::RTValCodecResult_Accept_Complete;
      append( cStrResult );
    }
  }
  else if ( isTrue )
  {
    result = FabricCore::RTValCodecResult_Accept_Pending;
  }

  Py_XDECREF( pyResult );

  PyGILState_Release( gstate );

  return result;
}

FabricCore::RTValCodecResult PythonRTValFromJSONDecoder::decodeRTValFromJSON(
  FabricCore::Context const &context,
  FabricCore::RTVal &rtVal,
  FabricCore::RTValCodecContext codecContext,
  char const *jsonData,
  uint32_t jsonSize,
  MetadataLookupFunctor const &metadataLookup
  )
{
  PyGILState_STATE gstate = PyGILState_Ensure();

  PyObject *pyJSONString = PyString_FromStringAndSize( jsonData, jsonSize );

  PyObject *pyArgs = PyTuple_New( 1 );
  PyTuple_SetItem( pyArgs, 0, pyJSONString );
  PyObject *pyResult = PyEval_CallObject( m_pyRTValFromJSONDecoder, pyArgs );
  Py_DECREF( pyArgs );

  FabricCore::RTValCodecResult result =
    FabricCore::RTValCodecResult_Reject;

  bool isNone = pyResult == Py_None;
  bool isTrue = pyResult == Py_True;
  bool isFalse = pyResult == Py_False;

  if ( isTrue )
  {
    result = FabricCore::RTValCodecResult_Accept_Pending;
  }
  else if ( isFalse || isNone )
  {
    // already set
  }
  else
  {
    rtVal.assign( PyObjectToRTVal( &context, pyResult ) );
    if ( !!rtVal )
      result = FabricCore::RTValCodecResult_Accept_Complete;
  }

  Py_XDECREF( pyResult );

  PyGILState_Release( gstate );

  return result;
}

void PythonSingleStringCallback(
  void *_pyReportCallback,
  char const *data,
  uint32_t length
  )
{
  PyGILState_STATE gstate = PyGILState_Ensure();

  PyObject *pyString = PyString_FromStringAndSize(
    data, length
    );

  PyObject *pyArgs = PyTuple_New( 1 );
  PyTuple_SetItem( pyArgs, 0, pyString );
  pyString = NULL;

  PyObject *pyReportCallback =
    reinterpret_cast<PyObject *>( _pyReportCallback );

  PyObject *pyResult =
    PyEval_CallObject( pyReportCallback, pyArgs );

  Py_XDECREF( pyResult );
  Py_DECREF( pyArgs );

  PyGILState_Release( gstate );
}

void PythonReportCallback(
  void *_pyReportCallback,
  FabricCore::ReportSource source,
  FabricCore::ReportLevel level,
  char const *lineData,
  uint32_t lineSize
  )
{
  PyGILState_STATE gstate = PyGILState_Ensure();

  PyObject *pyArgs = PyTuple_New( 3 );

  PyObject *pySource = PyInt_FromLong( source );
  PyTuple_SetItem( pyArgs, 0, pySource );
  pySource = NULL;
  PyObject *pyLevel = PyInt_FromLong( level );
  PyTuple_SetItem( pyArgs, 1, pyLevel );
  pyLevel = NULL;
  PyObject *pyLine = PyString_FromStringAndSize( lineData, lineSize );
  PyTuple_SetItem( pyArgs, 2, pyLine );
  pyLine = NULL;

  PyObject *pyReportCallback =
    reinterpret_cast<PyObject *>( _pyReportCallback );

  PyObject *pyResult =
    PyEval_CallObject( pyReportCallback, pyArgs );

  Py_XDECREF( pyResult );
  Py_DECREF( pyArgs );

  PyGILState_Release( gstate );
}

void PythonStatusCallback(
  void *userdata,
  char const *destinationData, uint32_t destinationLength,
  char const *payloadData, uint32_t payloadLength
  )
{
  PyGILState_STATE gstate = PyGILState_Ensure();
  PyObject *arglist = Py_BuildValue(
      "(s#,s#)",
      destinationData, destinationLength,
      payloadData, payloadLength
      );
  PyObject *callback = reinterpret_cast<PyObject *>( userdata );
  PyObject *result = PyEval_CallObject( callback, arglist );
  Py_DECREF( arglist );
  Py_XDECREF( result );
  PyGILState_Release( gstate );
}

typedef std::map<void *, bool> PyObjParents;
static void InitFECVariantFromPyObject( FEC_Variant *fecVariant, PyObject *obj, PyObjParents &parents )
{
  if ( parents[obj] )
    throw FabricCore::Exception( "Fabric: cannot convert PyObject containing circular reference" );
  parents[obj] = true;

  if ( PyList_Check( obj ) )
  {
    uint32_t size = PyList_Size( obj );
    FEC_Variant *elements = FEC_AllocVariants( size );

    for (uint32_t i=0; i<size; i++)
      InitFECVariantFromPyObject(
        &elements[i],
        PyList_GetItem( obj, i ),
        parents
        );

    FEC_VariantInitArrayTake( fecVariant, size, elements );
  }
  else if ( PyDict_Check( obj ) )
  {
    PyObject *key, *value;
    Py_ssize_t pos = 0;
    FEC_VariantInitDictEmpty( fecVariant );
    while ( PyDict_Next( obj, &pos, &key, &value ) )
    {
      FEC_Variant keyVariant, valueVariant;
      InitFECVariantFromPyObject( &keyVariant, key, parents );
      InitFECVariantFromPyObject( &valueVariant, value, parents );
      FEC_VariantSetDictKeyTakeValueTake(
        fecVariant,
        &keyVariant,
        &valueVariant
        );
    }
  }
  else if ( obj == Py_None )
  {
    FEC_VariantInitNull( fecVariant );
  }
  else if ( PyString_Check( obj ) || PyUnicode_Check( obj ) )
  {
    FEC_VariantInitStringCopy_cstr( fecVariant, PyString_AsString( obj ) );
  }
  else if ( PyFloat_Check( obj ) )
  {
    FEC_VariantInitFloat64( fecVariant, PyFloat_AS_DOUBLE( obj ) );
  }
  // [andrew 2012-11-07] bool derives from Int so this check must come first
  else if ( PyBool_Check( obj ) )
  {
    FEC_VariantInitBoolean( fecVariant, obj == Py_True );
  }
  else if ( PyInt_Check( obj ) )
  {
    // [andrew 2012-11-15] a PyInt can always fit into a signed int with
    // the bitness of the system (32 or 64)
    long svalue = PyInt_AsLong( obj );
    int32_t int32_t_value = int32_t( svalue );
    if ( long( int32_t_value ) == svalue )
      FEC_VariantInitSInt32( fecVariant, int32_t_value );
    else
      FEC_VariantInitSInt64( fecVariant, svalue );
  }
  else if ( PySet_Check( obj ) )
  {
    PyErr_SetString(
      PyExc_Exception,
      "Fabric: cannot pass Python 'set' type, did you mean to pass a list?"
      );
  }
  else if ( PyLong_Check( obj ) )
  {
    PY_LONG_LONG svalue = PyLong_AsLongLong( obj );
    if ( !PyErr_Occurred() )
      FEC_VariantInitSInt64( fecVariant, int64_t( svalue ) );
    else
    {
      PyErr_Clear();

      unsigned PY_LONG_LONG uvalue = PyLong_AsUnsignedLongLong( obj );
      if ( !PyErr_Occurred() )
        FEC_VariantInitUInt64( fecVariant, uint64_t( uvalue ) );
      else
      {
        PyErr_SetString(
          PyExc_Exception,
          "Fabric: numeric value too large for representation"
          );
      }
    }
  }
  else
  {
    PyObject *key, *value;
    Py_ssize_t pos = 0;
    FEC_VariantInitDictEmpty( fecVariant );
    PyObject *dict = PyObject_GetAttrString( obj, "__dict__" );
    if ( dict != NULL )
    {
      while ( PyDict_Next( dict, &pos, &key, &value ) )
      {
        FEC_Variant keyVariant, valueVariant;
        InitFECVariantFromPyObject( &keyVariant, key, parents );
        InitFECVariantFromPyObject( &valueVariant, value, parents );
        FEC_VariantSetDictKeyTakeValueTake(
          fecVariant,
          &keyVariant,
          &valueVariant
          );
      }
    }
    else
    {
      PyErr_SetString(
        PyExc_Exception,
        "Fabric: unknown Python type"
        );
    }
  }
  parents[obj] = false;
}

// [andrew 2012-11-15] exceptions do not propagate properly on functions
// that don't return a PyObject* and must return NULL on exception
PyObject *PyObjectToVariant( PyObject *obj, FabricCore::Variant *result )
{
  FEC_Variant fecVariant;
  PyObjParents parents;
  try
  {
    InitFECVariantFromPyObject( &fecVariant, obj, parents );
  }
  catch ( FabricCore::Exception e )
  {
    PyErr_SetString(
      PyExc_Exception,
      e.getDesc_cstr()
      );
    return NULL;
  }

  if ( PyErr_Occurred() )
    return NULL;
  result->take( &fecVariant );

  Py_INCREF( Py_None );
  return Py_None;
}

static PyObject *FECVariantToPyObject( FEC_Variant const *fecVariant )
{
  PyObject *obj;
  switch ( FEC_VariantGetType( fecVariant ) )
  {
    case FEC_VT_NULL:
      obj = Py_None;
      break;

    case FEC_VT_BOOLEAN:
      obj = PyBool_FromLong( long( FEC_VariantGetBoolean( fecVariant ) ) );
      break;

    case FEC_VT_UINT8:
      obj = PyInt_FromLong( long( FEC_VariantGetUInt8( fecVariant ) ) );
      break;

    case FEC_VT_SINT8:
      obj = PyInt_FromLong( long( FEC_VariantGetSInt8( fecVariant ) ) );
      break;

    case FEC_VT_UINT16:
      obj = PyInt_FromLong( long( FEC_VariantGetUInt16( fecVariant ) ) );
      break;

    case FEC_VT_SINT16:
      obj = PyInt_FromLong( long( FEC_VariantGetSInt16( fecVariant ) ) );
      break;

    case FEC_VT_UINT32:
      obj = PyInt_FromSize_t( size_t( FEC_VariantGetUInt32( fecVariant ) ) );
      break;

    case FEC_VT_SINT32:
      obj = PyInt_FromLong( long( FEC_VariantGetSInt32( fecVariant ) ) );
      break;

    case FEC_VT_UINT64:
      obj = PyLong_FromUnsignedLongLong( ( unsigned PY_LONG_LONG )( FEC_VariantGetUInt64( fecVariant ) ) );
      break;

    case FEC_VT_SINT64:
      obj = PyLong_FromLongLong( ( PY_LONG_LONG )( FEC_VariantGetSInt64( fecVariant ) ) );
      break;

    case FEC_VT_FLOAT32:
      obj = PyFloat_FromDouble( double( FEC_VariantGetFloat32( fecVariant ) ) );
      break;

    case FEC_VT_FLOAT64:
      obj = PyFloat_FromDouble( double( FEC_VariantGetFloat64( fecVariant ) ) );
      break;

    case FEC_VT_STRING:
      obj = PyString_FromStringAndSize(
        FEC_VariantGetStringData( fecVariant ),
        FEC_VariantGetStringLength( fecVariant )
        );
      break;

    case FEC_VT_ARRAY:
    {
      uint32_t size = FEC_VariantGetArraySize( fecVariant );
      obj = PyList_New( size );
      for (uint32_t i=0; i<size; i++)
      {
        FEC_Variant const *elementFECVariant = FEC_VariantGetArrayElement( fecVariant, i );
        try
        {
          PyObject *element = FECVariantToPyObject( elementFECVariant );
          if ( PyList_SetItem( obj, i, element ) != 0 )
            FabricCore::Exception::Throw( "error setting PyObject list element" );
        }
        catch ( FabricCore::Exception e )
        {
          Py_DECREF( obj );
          throw e;
        }
      }
    }
    break;

    case FEC_VT_DICT:
    {
      obj = PyDict_New();
      FEC_VariantDictIter fecDictIter;
      for (
        FEC_VariantDictIterInit( &fecDictIter, fecVariant );
        !FEC_VariantDictIterIsDone( &fecDictIter );
        FEC_VariantDictIterNext( &fecDictIter )
        )
      {
        FEC_Variant const *keyFECVariant = FEC_VariantDictIterGetKey( &fecDictIter );
        FEC_Variant const *valueFECVariant = FEC_VariantDictIterGetValue( &fecDictIter );
        try
        {
          PyObject *key = FECVariantToPyObject( keyFECVariant );
          PyObject *value = FECVariantToPyObject( valueFECVariant );
          if ( PyDict_SetItem( obj, key, value ) != 0 )
            FabricCore::Exception::Throw( "error setting PyObject dict element" );
        }
        catch ( FabricCore::Exception e )
        {
          FEC_VariantDictIterDispose( &fecDictIter );
          Py_DECREF( obj );
          throw e;
        }
      }
      FEC_VariantDictIterDispose( &fecDictIter );
    }
    break;

    default:
      FabricCore::Exception::Throw( "unexpected variant type" );
      obj = 0;
      break;
  }

  return obj;
}

PyObject *VariantToPyObject( FabricCore::Variant const *variant )
{
  return FECVariantToPyObject( variant->getFECVariant() );
}

////////////////////////////////////////////////////////////////////////////

static bool SetPyErrForFECException()
{
  char const *cString = FEC_GetLastExceptionCString();
  if ( cString )
  {
    PyErr_SetString( PyExc_AttributeError, cString );
    FEC_ClearLastException();
  }
  return !!cString;
}

static FEC_RTValRef PyObjectToFECRTValRef(
  FEC_ContextRef fecContextRef,
  PyObject *pyObject
  );

static PyObject *FEC_SimpleRTValToPyObject(
  FEC_ContextRef fecContextRef,
  FEC_RTValRef fecRTValRef
  );

static PyObject *FEC_RTValToPyObject(
  FEC_ContextRef fecContextRef,
  FEC_RTValRef fecRTValRef
  );

struct PyRTValMethodObject
{
  PyObject_HEAD
  FEC_ContextRef fecContextRef;
  FEC_RTValRef fecRTValRef;
  char *methodNameCString;
};

static void PyRTValMethodObject_dealloc(
  PyRTValMethodObject *pyRTValMethodObject
  )
{
  free( pyRTValMethodObject->methodNameCString );
  FEC_RefRelease( pyRTValMethodObject->fecRTValRef );
  FEC_RefRelease( pyRTValMethodObject->fecContextRef );
  // [pzion 20130730] This doesn't seem to be needed, even though it
  // is used in some examples??
  // PyMem_Del( pyRTTypeObj );
}

static PyObject *PyRTValMethodObject_repr(
  PyRTValMethodObject *pyRTValMethodObject
  )
{
  return PyString_FromFormat(
    "<RTVal.%s>",
    pyRTValMethodObject->methodNameCString
    );
}

static PyObject *PyRTValMethodObject_call(
  PyRTValMethodObject *pyRTValMethodObject,
  PyObject *arg,
  PyObject *kw
  )
{
  if ( strcmp( pyRTValMethodObject->methodNameCString, "getJSON" ) == 0 )
  {
    FEC_RTValRef resultFECRTValRef = FEC_RTValGetJSON(
        pyRTValMethodObject->fecRTValRef
        );

    if ( SetPyErrForFECException() )
      return 0;

    return FEC_RTValToPyObject(
      pyRTValMethodObject->fecContextRef,
      resultFECRTValRef
      );
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "setJSON" ) == 0 )
  {
    uint32_t numargs = PyTuple_Size( arg );
    if ( numargs != 1 )
    {
      PyErr_SetString(
        PyExc_RuntimeError,
        "setJSON takes a single parameter"
        );
      return 0;
    }

    FEC_RTValRef jsonVal = PyObjectToFECRTValRef(
      pyRTValMethodObject->fecContextRef,
      PyTuple_GetItem( arg, 0 )
      );
    if ( jsonVal == FEC_NULL_REF )
      return 0;

    FEC_RTValSetJSON(
      pyRTValMethodObject->fecRTValRef,
      jsonVal
      );

    if ( SetPyErrForFECException() )
      return 0;

    Py_RETURN_NONE;
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "getDesc" ) == 0 )
  {
    FEC_RTValRef resultFECRTValRef = FEC_RTValGetDesc(
        pyRTValMethodObject->fecRTValRef
        );

    if ( SetPyErrForFECException() )
      return 0;

    return FEC_RTValToPyObject(
      pyRTValMethodObject->fecContextRef,
      resultFECRTValRef
      );
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "getTypeName" ) == 0 )
  {
    FEC_RTValRef resultFECRTValRef = FEC_RTValGetTypeName(
        pyRTValMethodObject->fecRTValRef
        );

    if ( SetPyErrForFECException() )
      return 0;

    return FEC_RTValToPyObject(
      pyRTValMethodObject->fecContextRef,
      resultFECRTValRef
      );
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "isNullObject" ) == 0 )
  {
    bool result = FEC_RTValIsNullObject(
        pyRTValMethodObject->fecRTValRef
        );
    if ( SetPyErrForFECException() )
      return 0;
    if ( result )
      Py_RETURN_TRUE;
    Py_RETURN_FALSE;
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "isArray" ) == 0 )
  {
    bool result = FEC_RTValIsArray(
        pyRTValMethodObject->fecRTValRef
        );
    if ( SetPyErrForFECException() )
      return 0;
    if ( result )
      Py_RETURN_TRUE;
    Py_RETURN_FALSE;
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "isDict" ) == 0 )
  {
    bool result = FEC_RTValIsDict(
        pyRTValMethodObject->fecRTValRef
        );
    if ( SetPyErrForFECException() )
      return 0;
    if ( result )
      Py_RETURN_TRUE;
    Py_RETURN_FALSE;
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "isStruct" ) == 0 )
  {
    bool result = FEC_RTValIsStruct(
        pyRTValMethodObject->fecRTValRef
        );
    if ( SetPyErrForFECException() )
      return 0;
    if ( result )
      Py_RETURN_TRUE;
    Py_RETURN_FALSE;
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "isObject" ) == 0 )
  {
    bool result = FEC_RTValIsObject(
        pyRTValMethodObject->fecRTValRef
        );
    if ( SetPyErrForFECException() )
      return 0;
    if ( result )
      Py_RETURN_TRUE;
    Py_RETURN_FALSE;
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "isInterface" ) == 0 )
  {
    bool result = FEC_RTValIsInterface(
        pyRTValMethodObject->fecRTValRef
        );
    if ( SetPyErrForFECException() )
      return 0;
    if ( result )
      Py_RETURN_TRUE;
    Py_RETURN_FALSE;
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "getSimpleType" ) == 0 )
  {
    return FEC_SimpleRTValToPyObject(
      pyRTValMethodObject->fecContextRef,
      pyRTValMethodObject->fecRTValRef
      );
  }
  else if ( strcmp( pyRTValMethodObject->methodNameCString, "isExEQTo" ) == 0 )
  {
    uint32_t numargs = PyTuple_Size( arg );
    if ( numargs != 1 )
    {
      PyErr_SetString(
        PyExc_RuntimeError,
        "isExEQTo() takes a single parameter"
        );
      return 0;
    }

    FEC_RTValRef otherRTValRef = PyObjectToFECRTValRef(
      pyRTValMethodObject->fecContextRef, PyTuple_GetItem( arg, 0 ) );
    if ( otherRTValRef == FEC_NULL_REF )
    {
      PyErr_SetString(
        PyExc_RuntimeError,
        "parameter to isExEQTo() is not an RTVal"
        );
      return 0;
    }

    int result =
      FEC_RTValIsExEQTo( pyRTValMethodObject->fecRTValRef, otherRTValRef );
    FEC_RefRelease( otherRTValRef );

    return PyBool_FromLong( result );
  }

  if ( FEC_RTValIsArray( pyRTValMethodObject->fecRTValRef ) )
  {
    if ( strcmp( pyRTValMethodObject->methodNameCString, "resize" ) == 0 )
    {
      uint32_t numargs = PyTuple_Size( arg );
      if ( numargs != 1 )
      {
        PyErr_SetString(
          PyExc_RuntimeError,
          "resize takes a single parameter"
          );
        return 0;
      }

      PyObject *sizeArg = PyTuple_GetItem( arg, 0 );
      if ( !PyInt_Check( sizeArg ) )
      {
        PyErr_SetString(
          PyExc_RuntimeError,
          "resize parameter must be an integer"
          );
        return 0;
      }

      long longSize = PyInt_AsLong( sizeArg );
      uint32_t size = uint32_t( longSize );
      if ( long( size ) != longSize )
      {
        PyErr_Format(
          PyExc_RuntimeError,
          "size too large: %ld",
          longSize
          );
        return 0;
      }

      FEC_RTValSetArraySize( pyRTValMethodObject->fecRTValRef, size );
      if ( SetPyErrForFECException() )
        return 0;

      Py_RETURN_NONE;
    }

    PyErr_Format(
      PyExc_RuntimeError,
      "%s: cannot call method on array object",
      pyRTValMethodObject->methodNameCString
      );
    return 0;
  }

  char const *resultTypeCString;
  uint32_t rtValRefCount;
  FEC_RTValRef *rtValRefs;
  if ( PyTuple_Check( arg ) )
  {
    rtValRefCount = PyTuple_Size( arg );
    if ( rtValRefCount < 1 )
    {
      PyErr_SetString(
        PyExc_RuntimeError,
        "first parameter to method call must be result type (or empty string)"
        );
      return 0;
    }
    PyObject *resultTypePyObject = PyTuple_GetItem( arg, 0 );
    if ( !PyString_Check( resultTypePyObject )
      && !PyUnicode_Check( resultTypePyObject )
      )
    {
      PyErr_SetString(
        PyExc_RuntimeError,
        "first parameter to method call must be result type (or empty string)"
        );
      return 0;
    }
    resultTypeCString = PyString_AsString( resultTypePyObject );
    --rtValRefCount;
    rtValRefs = (FEC_RTValRef *)alloca(
      rtValRefCount * sizeof(FEC_RTValRef )
      );
    for ( uint32_t i=0; i<rtValRefCount; ++i )
      rtValRefs[i] = PyObjectToFECRTValRef(
        pyRTValMethodObject->fecContextRef,
        PyTuple_GetItem( arg, i + 1 )
        );
  }
  else
  {
    PyErr_SetString(
      PyExc_RuntimeError,
      "first parameter to method call must be result type (or empty string)"
      );
    return 0;
  }

  FEC_RTValRef resultFECRTValRef =
    FEC_RTValCallMethod(
      resultTypeCString,
      pyRTValMethodObject->fecRTValRef,
      pyRTValMethodObject->methodNameCString,
      rtValRefCount,
      rtValRefs
      );

  for ( uint32_t i=0; i<rtValRefCount; ++i )
    FEC_RefRelease( rtValRefs[i] );

  if ( SetPyErrForFECException() )
    return 0;

  if ( resultFECRTValRef == FEC_NULL_REF )
    Py_RETURN_NONE;

  return FEC_RTValToPyObject(
    pyRTValMethodObject->fecContextRef,
    resultFECRTValRef
    );
}

static PyTypeObject PyRTValMethodObjectType =
{
  PyObject_HEAD_INIT(0)
  0,                    /*ob_size*/
  "PyRTValMethodObject",                    /*tp_name*/
  sizeof(PyRTValMethodObject),      /*tp_basicsize*/
  0,                    /*tp_itemsize*/
  /* methods */
  (destructor)PyRTValMethodObject_dealloc,
  0,                    /*tp_print*/
  0, //(getattrfunc)PyRTValMethodObject_getattr,
  0, //(setattrfunc)PyRTTypesObject_setattr,
  0,                    /*tp_compare/tp_reserved*/
  (reprfunc)PyRTValMethodObject_repr,
  0,                    /*tp_as_number*/
  0,                    /*tp_as_sequence*/
  0,                    /*tp_as_mapping*/
  0,                    /*tp_hash*/
  (ternaryfunc)PyRTValMethodObject_call,
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,        /*tp_flags*/
};

struct PyRTValObject
{
  PyObject_HEAD
  FEC_ContextRef fecContextRef;
  FEC_RTValRef fecRTValRef;
  PyObject *dict;
};

static void PyRTValObject_dealloc( PyRTValObject *pyRTValObject )
{
  FEC_RefRelease( pyRTValObject->fecRTValRef );
  FEC_RefRelease( pyRTValObject->fecContextRef );
  // [pzion 20130730] This doesn't seem to be needed, even though it
  // is used in some examples??
  // PyMem_Del( pyRTValObject );
}

static PyObject *PyRTValObject_getattr(
  PyRTValObject *pyRTValObject,
  char *name
  )
{
  FEC_ClearLastException();

  if ( !pyRTValObject )
    return 0;

  FEC_RTValRef attrRTValRef = FEC_NULL_REF;
  if ( FEC_RTValIsObject( pyRTValObject->fecRTValRef ) ||
       FEC_RTValIsStruct( pyRTValObject->fecRTValRef ) )
  {
    attrRTValRef = FEC_RTValMaybeGetMemberRef(
      pyRTValObject->fecRTValRef,
      name
      );

    if ( SetPyErrForFECException() )
      return 0;
  }

  if ( attrRTValRef != FEC_NULL_REF )
    return FEC_RTValToPyObject(
      pyRTValObject->fecContextRef,
      attrRTValRef
      );

  PyRTValMethodObject *pyRTValMethodObject = reinterpret_cast<PyRTValMethodObject *>(
    PyObject_CallObject(
      reinterpret_cast<PyObject *>( &PyRTValMethodObjectType ),
      NULL
      )
    );
  if ( pyRTValMethodObject )
  {
    pyRTValMethodObject->fecContextRef = pyRTValObject->fecContextRef;
    FEC_RefRetain( pyRTValMethodObject->fecContextRef );
    pyRTValMethodObject->fecRTValRef = pyRTValObject->fecRTValRef;
    FEC_RefRetain( pyRTValMethodObject->fecRTValRef );
    pyRTValMethodObject->methodNameCString = strdup( name );
  }
  return (PyObject *)pyRTValMethodObject;
}

static int PyRTValObject_setattr(
  PyRTValObject *pyRTValObject,
  char *name,
  PyObject *valuePyObject
  )
{
  if ( !pyRTValObject )
    return -1;

  FEC_RTValRef valueRTValRef =
    PyObjectToFECRTValRef( pyRTValObject->fecContextRef, valuePyObject );
  FEC_RTValSetMember(
    pyRTValObject->fecRTValRef,
    name,
    valueRTValRef
    );
  FEC_RefRelease( valueRTValRef );

  if ( SetPyErrForFECException() )
    return -1;

  return 0;
}

static PyObject *PyRTValObject_repr(PyRTValObject *pyRTValObject)
{
  if ( pyRTValObject->fecRTValRef == FEC_NULL_REF )
    return PyString_FromString( "<RTVal:null>" );

  FEC_RTValRef descRTValRef
    = FEC_RTValGetDesc(
      pyRTValObject->fecRTValRef
      );

  PyObject *resultPyObject =
    PyString_FromFormat(
      "<RTVal:%s>",
      FEC_RTValGetStringCString( descRTValRef )
      );

  FEC_RefRelease( descRTValRef );

  return resultPyObject;
}

static Py_ssize_t PyRTValObject_length( PyRTValObject* self )
{
  if ( FEC_RTValIsArray( self->fecRTValRef ) )
  {
    uint32_t len = FEC_RTValGetArraySize( self->fecRTValRef );
    if ( SetPyErrForFECException() )
      return -1;
    return len;
  }
  else if ( FEC_RTValIsDict( self->fecRTValRef ) )
  {
    uint32_t len = FEC_RTValGetDictSize( self->fecRTValRef );
    if ( SetPyErrForFECException() )
      return -1;
    return len;
  }

  PyErr_Format(
    PyExc_RuntimeError,
    "unsupported for this type"
    );
  return -1;
}

static PyObject *PyRTValObject_getitem( PyRTValObject *self, PyObject *key )
{
  if ( FEC_RTValIsArray( self->fecRTValRef ) )
  {
    if ( PyInt_Check( key ) )
    {
      long longIndex = PyInt_AsLong( key );
      uint32_t index = uint32_t( longIndex );
      if ( long( index ) != longIndex )
      {
        PyErr_Format(
          PyExc_RuntimeError,
          "index too large: %ld",
          longIndex
          );
        return 0;
      }
      FEC_RTValRef rval = FEC_RTValGetArrayElementRef( self->fecRTValRef, index );
      if ( SetPyErrForFECException() )
        return 0;
      return FEC_RTValToPyObject(
        self->fecContextRef,
        rval
        );
    }

    PyErr_Format(
      PyExc_RuntimeError,
      "incompatible index type, must be integer"
      );
    return 0;
  }
  else if ( FEC_RTValIsDict( self->fecRTValRef ) )
  {
    FEC_RTValRef keyFECRTValueRef =
      PyObjectToFECRTValRef( self->fecContextRef, key );
    if ( keyFECRTValueRef == FEC_NULL_REF )
      return 0;

    FEC_RTValRef valueFECRTValRef =
      FEC_RTValGetDictElement( self->fecRTValRef, keyFECRTValueRef );
    if ( SetPyErrForFECException() )
      return 0;

    return FEC_RTValToPyObject(
      self->fecContextRef,
      valueFECRTValRef
      );
  }

  PyErr_Format(
    PyExc_RuntimeError,
    "unsupported for this type"
    );
  return 0;
}

static int PyRTValObject_setitem( PyRTValObject *self, PyObject *key, PyObject *value );

static PyMappingMethods PyRTValObject_as_mapping =
{
  (lenfunc)PyRTValObject_length,
  (binaryfunc)PyRTValObject_getitem,
  (objobjargproc)PyRTValObject_setitem
};

static PyTypeObject PyRTValObjectType =
{
  PyObject_HEAD_INIT(0)
  0,                    /*ob_size*/
  "PyRTValObject",                    /*tp_name*/
  sizeof(PyRTValObject),      /*tp_basicsize*/
  0,                    /*tp_itemsize*/
  /* methods */
  (destructor)PyRTValObject_dealloc,
  0,                    /*tp_print*/
  (getattrfunc)PyRTValObject_getattr,
  (setattrfunc)PyRTValObject_setattr,
  0,                    /*tp_compare/tp_reserved*/
  (reprfunc)PyRTValObject_repr,
  0,                    /*tp_as_number*/
  0,                    /*tp_as_sequence*/
  &PyRTValObject_as_mapping, /*tp_as_mapping*/
  0,                    /*tp_hash*/
  0, //(ternaryfunc)PyRTValObject_call, /*tp_call*/
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,        /*tp_flags*/
};

static int PyRTValObject_setitem( PyRTValObject *self, PyObject *key, PyObject *value )
{
  if ( FEC_RTValIsArray( self->fecRTValRef ) )
  {
    if ( PyInt_Check( key ) )
    {
      long longIndex = PyInt_AsLong( key );
      uint32_t index = uint32_t( longIndex );
      if ( long( index ) != longIndex )
      {
        PyErr_Format(
          PyExc_RuntimeError,
          "index too large: %ld",
          longIndex
          );
        return -1;
      }

      FEC_RTValRef rtval = PyObjectToFECRTValRef( self->fecContextRef, value );
      if ( rtval == FEC_NULL_REF )
        return -1;

      FEC_RTValSetArrayElement( self->fecRTValRef, index, rtval );
      if ( SetPyErrForFECException() )
        return -1;
      return 0;
    }

    PyErr_Format(
      PyExc_RuntimeError,
      "incompatible index type, must be integer"
      );
    return -1;
  }
  else if ( FEC_RTValIsDict( self->fecRTValRef ) )
  {
    FEC_RTValRef keyRTVal = PyObjectToFECRTValRef( self->fecContextRef, key );
    if ( keyRTVal == FEC_NULL_REF )
      return -1;

    FEC_RTValRef valueRTVal = PyObjectToFECRTValRef( self->fecContextRef, value );
    if ( valueRTVal == FEC_NULL_REF )
      return -1;

    FEC_RTValSetDictElement( self->fecRTValRef, keyRTVal, valueRTVal );
    if ( SetPyErrForFECException() )
      return -1;
    return 0;
  }

  PyErr_Format(
    PyExc_RuntimeError,
    "unsupported for this type"
    );
  return -1;
}

static PyObject *FEC_SimpleRTValToPyObject(
  FEC_ContextRef fecContextRef,
  FEC_RTValRef fecRTValRef
  )
{
  FEC_RTValSimpleData simpleData;
  if ( FEC_RTValMaybeGetSimpleData( fecRTValRef, &simpleData ) )
  {
    switch ( simpleData.type )
    {
    case FEC_RTVAL_SIMPLE_TYPE_BOOLEAN:
      return PyBool_FromLong( long( simpleData.value.boolean ) );

    case FEC_RTVAL_SIMPLE_TYPE_UINT8:
      return PyInt_FromLong( long( simpleData.value.uint8 ) );

    case FEC_RTVAL_SIMPLE_TYPE_SINT8:
      return PyInt_FromLong( long( simpleData.value.sint8 ) );

    case FEC_RTVAL_SIMPLE_TYPE_UINT16:
      return PyInt_FromLong( long( simpleData.value.uint16 ) );

    case FEC_RTVAL_SIMPLE_TYPE_SINT16:
      return PyInt_FromLong( long( simpleData.value.sint16 ) );

    case FEC_RTVAL_SIMPLE_TYPE_UINT32:
      return PyInt_FromSize_t( size_t( simpleData.value.uint32 ) );

    case FEC_RTVAL_SIMPLE_TYPE_SINT32:
      return PyInt_FromLong( long( simpleData.value.sint32 ) );

    case FEC_RTVAL_SIMPLE_TYPE_UINT64:
      return PyLong_FromUnsignedLongLong( ( unsigned PY_LONG_LONG )( simpleData.value.uint64 ) );

    case FEC_RTVAL_SIMPLE_TYPE_SINT64:
      return PyLong_FromLongLong( ( PY_LONG_LONG )( simpleData.value.sint64 ) );

    case FEC_RTVAL_SIMPLE_TYPE_FLOAT32:
      return PyFloat_FromDouble( double( simpleData.value.float32 ) );

    case FEC_RTVAL_SIMPLE_TYPE_FLOAT64:
      return PyFloat_FromDouble( double( simpleData.value.float64 ) );

    case FEC_RTVAL_SIMPLE_TYPE_STRING:
      return PyString_FromStringAndSize(
        simpleData.value.string.cStringData,
        simpleData.value.string.length
        );
    }
  }
  Py_RETURN_NONE;
}

static PyObject *FEC_RTValToPyObject(
  FEC_ContextRef fecContextRef,
  FEC_RTValRef fecRTValRef
  )
{
  PyRTValObject *pyRTValObject = reinterpret_cast<PyRTValObject *>(
    PyObject_CallObject(
      reinterpret_cast<PyObject *>( &PyRTValObjectType ),
      NULL
      )
    );
  if ( pyRTValObject )
  {
    pyRTValObject->fecContextRef = fecContextRef;
    FEC_RefRetain( fecContextRef );
    pyRTValObject->fecRTValRef = fecRTValRef;
    FEC_RefRetain( fecRTValRef );
  }
  return (PyObject *)pyRTValObject;
}

PyObject *RTValToPyObject(
  FabricCore::Context const *context,
  FabricCore::RTVal const *rtVal
  )
{
  return FEC_RTValToPyObject(
    context->getFECContextRef(),
    rtVal->getFECRTValRef()
    );
}

PyObject *RTValToPyObject_DFGHost(
  FabricCore::DFGHost const *dfgHost,
  FabricCore::RTVal const *rtVal
  )
{
  FEC_ContextRef fecContextRef =
    FEC_DFGHostGetContext( dfgHost->getFECDFGHostRef() );
  PyObject *result =
    FEC_RTValToPyObject(
      fecContextRef,
      rtVal->getFECRTValRef()
      );
  FEC_RefRelease( fecContextRef );
  return result;
}

PyObject *RTValToPyObject_DFGExec(
  FabricCore::DFGExec const *dfgExec,
  FabricCore::RTVal const *rtVal
  )
{
  FEC_DFGHostRef fecDFGHostRef =
    FEC_DFGExecGetHost( dfgExec->getFECDFGExecRef() );
  FEC_ContextRef fecContextRef =
    FEC_DFGHostGetContext( fecDFGHostRef );
  FEC_RefRelease( fecDFGHostRef );

  PyObject *result;
  FEC_RTValRef rtValRef = rtVal->getFECRTValRef();

  PyObject *maybeSimpleType = Py_None;
  if ( rtValRef )
    maybeSimpleType = FEC_SimpleRTValToPyObject( fecContextRef, rtValRef );

  if ( maybeSimpleType != Py_None )
    result = maybeSimpleType;
  else
    result = FEC_RTValToPyObject( fecContextRef, rtValRef );
  FEC_RefRelease( fecContextRef );
  return result;
}

PyObject *RTValToPyObject_DFGBinding(
  FabricCore::DFGBinding const *dfgBinding,
  FabricCore::RTVal const *rtVal
  )
{
  FEC_DFGHostRef fecDFGHostRef =
    FEC_DFGBindingGetHost( dfgBinding->getFECDFGBindingRef() );
  FEC_ContextRef fecContextRef =
    FEC_DFGHostGetContext( fecDFGHostRef );
  FEC_RefRelease( fecDFGHostRef );
  PyObject *result =
    FEC_RTValToPyObject(
      fecContextRef,
      rtVal->getFECRTValRef()
      );
  FEC_RefRelease( fecContextRef );
  return result;
}

static FEC_RTValRef PyObjectToFECRTValRef(
  FEC_ContextRef fecContextRef,
  PyObject *pyObject
  )
{
  if ( Py_TYPE( pyObject ) == &PyRTValObjectType )
  {
    PyRTValObject *pyRTValObject = (PyRTValObject *)pyObject;
    FEC_RTValRef fecRTValRef = pyRTValObject->fecRTValRef;
    FEC_RefRetain( fecRTValRef );
    return fecRTValRef;
  }
  else if ( PyString_Check( pyObject ) || PyUnicode_Check( pyObject ) )
  {
    char const *cString = PyString_AsString( pyObject );
    uint32_t length = strlen( cString );
    return FEC_RTConstructValueFromString(
      fecContextRef, cString, length
      );
  }
  else if ( PyFloat_Check( pyObject ) )
  {
    return FEC_RTConstructValueFromFloat64(
      fecContextRef, PyFloat_AS_DOUBLE( pyObject )
      );
  }
  // [andrew 2012-11-07] bool derives from Int so this check must come first
  else if ( PyBool_Check( pyObject ) )
  {
    return FEC_RTConstructValueFromBoolean(
      fecContextRef, pyObject == Py_True
      );
  }
  else if ( PyInt_Check( pyObject ) )
  {
    // [andrew 2012-11-15] a PyInt can always fit into a signed int with
    // the bitness of the system (32 or 64)
    long longValue = PyInt_AsLong( pyObject );
    int32_t sint32Value = int32_t( longValue );
    if ( long( sint32Value ) == longValue )
      return FEC_RTConstructValueFromSInt32( fecContextRef, sint32Value );
    else
      return FEC_RTConstructValueFromSInt64( fecContextRef, longValue );
  }
  else if ( PyLong_Check( pyObject ) )
  {
    int64_t sint64 = PyLong_AsLongLong( pyObject );
    if ( !PyErr_Occurred() )
      return FEC_RTConstructValueFromSInt64( fecContextRef, sint64 );
    else
    {
      PyErr_Clear();

      uint64_t uint64 = PyLong_AsUnsignedLongLong( pyObject );
      if ( !PyErr_Occurred() )
        return FEC_RTConstructValueFromUInt64( fecContextRef, uint64 );
      else
      {
        PyErr_SetString(
          PyExc_Exception,
          "numeric value too large for Creation Platform representation"
          );
        return FEC_NULL_REF;
      }
    }
  }

  PyErr_SetString(
    PyExc_AttributeError,
    "PyObjectToRTVal: unhandled type"
    );

  return FEC_NULL_REF;
}

FabricCore::RTVal PyObjectToRTVal(
  FabricCore::Context const *context,
  PyObject *pyObject
  )
{
  return FabricCore::RTVal(
    PyObjectToFECRTValRef(
      context->getFECContextRef(),
      pyObject
      )
    );
}

FabricCore::RTVal PyObjectToRTVal_DFGHost(
  FabricCore::DFGHost const *dfgHost,
  PyObject *pyObject
  )
{
  FEC_ContextRef fecContextRef =
    FEC_DFGHostGetContext( dfgHost->getFECDFGHostRef() );
  FabricCore::RTVal result(
    PyObjectToFECRTValRef(
      fecContextRef,
      pyObject
      )
    );
  FEC_RefRelease( fecContextRef );
  return result;
}

FabricCore::RTVal PyObjectToRTVal_DFGExec(
  FabricCore::DFGExec const *dfgExec,
  PyObject *pyObject
  )
{
  FEC_DFGHostRef fecDFGHostRef =
    FEC_DFGExecGetHost( dfgExec->getFECDFGExecRef() );
  FEC_ContextRef fecContextRef =
    FEC_DFGHostGetContext( fecDFGHostRef );
  FEC_RefRelease( fecDFGHostRef );
  FabricCore::RTVal result(
    PyObjectToFECRTValRef(
      fecContextRef,
      pyObject
      )
    );
  FEC_RefRelease( fecContextRef );
  return result;
}

FabricCore::RTVal PyObjectToRTVal_DFGBinding(
  FabricCore::DFGBinding const *dfgBinding,
  PyObject *pyObject
  )
{
  FEC_DFGHostRef fecDFGHostRef =
    FEC_DFGBindingGetHost( dfgBinding->getFECDFGBindingRef() );
  FEC_ContextRef fecContextRef =
    FEC_DFGHostGetContext( fecDFGHostRef );
  FEC_RefRelease( fecDFGHostRef );
  FabricCore::RTVal result(
    PyObjectToFECRTValRef(
      fecContextRef,
      pyObject
      )
    );
  FEC_RefRelease( fecContextRef );
  return result;
}

// --- PyRTTypeMethodObject

struct PyRTTypeMethodObject
{
  PyObject_HEAD
  FEC_ContextRef fecContextRef;
  char *typeNameCString;
  char *methodNameCString;
};

static void PyRTTypeMethodObject_dealloc(
  PyRTTypeMethodObject *pyRTTypeMethodObject
  )
{
  free( pyRTTypeMethodObject->methodNameCString );
  free( pyRTTypeMethodObject->typeNameCString );
  FEC_RefRelease( pyRTTypeMethodObject->fecContextRef );
  // [pzion 20130730] This doesn't seem to be needed, even though it
  // is used in some examples??
  // PyMem_Del( pyRTTypeObj );
}

static PyObject *PyRTTypeMethodObject_repr(
  PyRTTypeMethodObject *pyRTTypeMethodObject
  )
{
  return PyString_FromFormat(
    "<RT:%s.%s>",
    pyRTTypeMethodObject->typeNameCString,
    pyRTTypeMethodObject->methodNameCString
    );
}

static PyObject *PyRTTypeMethodObject_call(
  PyRTTypeMethodObject *pyRTTypeMethodObject,
  PyObject *arg,
  PyObject *kw
  )
{
  if ( strcmp( pyRTTypeMethodObject->methodNameCString, "create" ) == 0 )
  {
    uint32_t argCount;
    FEC_RTValRef *argRTValRefs;
    if ( PyTuple_Check( arg ) )
    {
      argCount = PyTuple_Size( arg );
      argRTValRefs = (FEC_RTValRef *)alloca(
        argCount * sizeof(FEC_RTValRef )
        );
      for ( uint32_t i=0; i<argCount; ++i )
        argRTValRefs[i] = PyObjectToFECRTValRef(
          pyRTTypeMethodObject->fecContextRef,
          PyTuple_GetItem( arg, i )
          );
    }
    else
    {
      argCount = 0;
      argRTValRefs = 0;
    }

    FEC_RTValRef resultFECRTValRef =
      FEC_RTCreateValue(
        pyRTTypeMethodObject->fecContextRef,
        pyRTTypeMethodObject->typeNameCString,
        argCount,
        argRTValRefs
        );

    for ( uint32_t i=0; i<argCount; ++i )
      FEC_RefRelease( argRTValRefs[i] );

    if ( SetPyErrForFECException() )
      return 0;

    return FEC_RTValToPyObject(
      pyRTTypeMethodObject->fecContextRef,
      resultFECRTValRef
      );
  }
  else if ( strcmp( pyRTTypeMethodObject->methodNameCString, "createArray" ) == 0 )
  {
    uint32_t arraySize = 0;
    FEC_RTValRef *arrayRTValRefs;
    if ( PyTuple_Check( arg ) )
    {
      uint32_t argCount = PyTuple_Size( arg );
      if ( argCount > 1 )
      {
        PyErr_Format(
          PyExc_RuntimeError,
          "invalid number of arguments"
          );
        return 0;
      }

      if ( argCount == 1 )
      {
        PyObject *pyArray = PyTuple_GetItem( arg, 0 );
        if ( !PyList_Check( pyArray ) )
        {
          PyErr_Format(
            PyExc_RuntimeError,
            "argument must be an array"
            );
          return 0;
        }

        arraySize = PyList_Size( pyArray );
        arrayRTValRefs = (FEC_RTValRef *)alloca(
          arraySize * sizeof(FEC_RTValRef )
          );
        for ( uint32_t i=0; i<arraySize; ++i )
          arrayRTValRefs[i] = PyObjectToFECRTValRef(
            pyRTTypeMethodObject->fecContextRef,
            PyList_GetItem( pyArray, i )
            );
      }
    }

    FEC_RTValRef rtvalArray = FEC_RTConstructVariableArrayValue(
      pyRTTypeMethodObject->fecContextRef,
      pyRTTypeMethodObject->typeNameCString
      );
    FEC_RTValSetArraySize( rtvalArray, arraySize );

    for ( uint32_t i=0; i<arraySize; ++i )
      FEC_RTValSetArrayElement( rtvalArray, i, arrayRTValRefs[i] );

    for ( uint32_t i=0; i<arraySize; ++i )
      FEC_RefRelease( arrayRTValRefs[i] );

    if ( SetPyErrForFECException() )
      return 0;

    return FEC_RTValToPyObject(
      pyRTTypeMethodObject->fecContextRef,
      rtvalArray
      );
  }
  else if ( strcmp( pyRTTypeMethodObject->methodNameCString, "createDict" ) == 0 )
  {
    uint32_t dictSize = 0;
    FEC_RTValRef *dictRTValKeys;
    FEC_RTValRef *dictRTValValues;

    if ( !PyTuple_Check( arg ) )
    {
      PyErr_Format(
        PyExc_RuntimeError,
        "createDict() requires the key type as a parameter"
        );
      return 0;
    }

    uint32_t argCount = PyTuple_Size( arg );
    if ( argCount < 1 || argCount > 2 )
    {
      PyErr_Format(
        PyExc_RuntimeError,
        "invalid number of arguments"
        );
      return 0;
    }

    PyObject *pyKeyType = PyTuple_GetItem( arg, 0 );
    if ( !PyString_Check( pyKeyType ) && !PyUnicode_Check( pyKeyType ) )
    {
      PyErr_Format(
        PyExc_RuntimeError,
        "invalid key type name, must be String"
        );
      return 0;
    }

    if ( argCount == 2 )
    {
      PyObject *pyDict = PyTuple_GetItem( arg, 1 );
      if ( !PyDict_Check( pyDict ) )
      {
        PyErr_Format(
          PyExc_RuntimeError,
          "argument must be an dict"
          );
        return 0;
      }

      dictSize = PyDict_Size( pyDict );
      dictRTValKeys = (FEC_RTValRef *)alloca(
        dictSize * sizeof( FEC_RTValRef )
        );
      dictRTValValues = (FEC_RTValRef *)alloca(
        dictSize * sizeof( FEC_RTValRef )
        );

      PyObject *pyKey, *pyValue;
      Py_ssize_t iterpos = 0;
      int i = 0;
      while ( PyDict_Next( pyDict, &iterpos, &pyKey, &pyValue ) )
      {
        dictRTValKeys[i] = PyObjectToFECRTValRef(
          pyRTTypeMethodObject->fecContextRef,
          pyKey
          );
        dictRTValValues[i] = PyObjectToFECRTValRef(
          pyRTTypeMethodObject->fecContextRef,
          pyValue
          );
        i++;
      }
    }

    char const *keyTypeCStr = PyString_AsString( pyKeyType );

    FEC_RTValRef rtvalDict = FEC_RTConstructDictValue(
      pyRTTypeMethodObject->fecContextRef,
      keyTypeCStr,
      pyRTTypeMethodObject->typeNameCString
      );

    for ( uint32_t i=0; i<dictSize; ++i )
      FEC_RTValSetDictElement( rtvalDict, dictRTValKeys[i], dictRTValValues[i] );

    for ( uint32_t i=0; i<dictSize; ++i )
    {
      FEC_RefRelease( dictRTValKeys[i] );
      FEC_RefRelease( dictRTValValues[i] );
    }

    if ( SetPyErrForFECException() )
      return 0;

    return FEC_RTValToPyObject(
      pyRTTypeMethodObject->fecContextRef,
      rtvalDict
      );
  }

  PyErr_Format(
    PyExc_RuntimeError,
    "'%s.%s': no such method",
    pyRTTypeMethodObject->typeNameCString,
    pyRTTypeMethodObject->methodNameCString
    );
  return 0;
}

static PyTypeObject PyRTTypeMethodObjectType =
{
  PyObject_HEAD_INIT(0)
  0,                    /*ob_size*/
  "PyRTTypeMethodObject",                    /*tp_name*/
  sizeof(PyRTTypeMethodObject),      /*tp_basicsize*/
  0,                    /*tp_itemsize*/
  /* methods */
  (destructor)PyRTTypeMethodObject_dealloc,
  0,                    /*tp_print*/
  0, //(getattrfunc)PyRTTypeMethodObject_getattr,
  0, //(setattrfunc)PyRTTypesObject_setattr,
  0,                    /*tp_compare/tp_reserved*/
  (reprfunc)PyRTTypeMethodObject_repr,
  0,                    /*tp_as_number*/
  0,                    /*tp_as_sequence*/
  0,                    /*tp_as_mapping*/
  0,                    /*tp_hash*/
  (ternaryfunc)PyRTTypeMethodObject_call,
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,        /*tp_flags*/
};

// --- PyRTTypeObject

struct PyRTTypeObject
{
  PyObject_HEAD
  FEC_Ref fecContextRef;
  char *userNameCString;
};

static void PyRTTypeObject_dealloc( PyRTTypeObject *pyRTTypeObj )
{
  free( pyRTTypeObj->userNameCString );
  FEC_RefRelease( pyRTTypeObj->fecContextRef );
  // [pzion 20130730] This doesn't seem to be needed, even though it
  // is used in some examples??
  // PyMem_Del( pyRTTypeObj );
}

static PyObject *PyRTTypeObject_repr( PyRTTypeObject *pyRTTypeObj )
{
  return PyString_FromFormat(
    "<RT:%s>",
    pyRTTypeObj->userNameCString
    );
}

static PyObject *PyRTTypeObject_getattr(
  PyRTTypeObject *pyRTTypeObject,
  char *name
  )
{
  FEC_ClearLastException();

  if ( !pyRTTypeObject )
    return 0;

  PyRTTypeMethodObject *pyRTTypeMethodObject = reinterpret_cast<PyRTTypeMethodObject *>(
    PyObject_CallObject(
      reinterpret_cast<PyObject *>( &PyRTTypeMethodObjectType ),
      NULL
      )
    );
  if ( pyRTTypeMethodObject )
  {
    pyRTTypeMethodObject->fecContextRef =
      pyRTTypeObject->fecContextRef;
    FEC_RefRetain( pyRTTypeMethodObject->fecContextRef );
    pyRTTypeMethodObject->typeNameCString =
      strdup( pyRTTypeObject->userNameCString );
    pyRTTypeMethodObject->methodNameCString = strdup( name );
  }
  return (PyObject *)pyRTTypeMethodObject;
}

static PyObject *PyRTTypeObject_call(
  PyRTTypeObject *pyRTTypeObj, PyObject *arg, PyObject *kw
  )
{
  uint32_t rtValRefCount;
  FEC_RTValRef *rtValRefs;
  if ( arg == Py_None )
  {
    rtValRefCount = 0;
    rtValRefs = 0;
  }
  else if ( PyTuple_Check( arg ) )
  {
    rtValRefCount = PyTuple_Size( arg );
    rtValRefs = (FEC_RTValRef *)alloca(
      rtValRefCount * sizeof(FEC_RTValRef )
      );
    for ( uint32_t i=0; i<rtValRefCount; ++i )
      rtValRefs[i] = PyObjectToFECRTValRef(
        pyRTTypeObj->fecContextRef, PyTuple_GetItem( arg, i )
        );
  }
  else
  {
    rtValRefCount = 1;
    rtValRefs = (FEC_RTValRef *)alloca(
      rtValRefCount * sizeof(FEC_RTValRef )
      );
    rtValRefs[0] = PyObjectToFECRTValRef(
      pyRTTypeObj->fecContextRef, arg
      );
  }

  FEC_RTValRef resultFECRTValRef =
    FEC_RTConstructValue(
      pyRTTypeObj->fecContextRef,
      pyRTTypeObj->userNameCString,
      rtValRefCount,
      rtValRefs
      );

  for ( uint32_t i=0; i<rtValRefCount; ++i )
    FEC_RefRelease( rtValRefs[i] );

  if ( SetPyErrForFECException() )
    return 0;

  return FEC_RTValToPyObject( pyRTTypeObj->fecContextRef, resultFECRTValRef );
}

static PyTypeObject PyRTTypeObjectType =
{
  PyObject_HEAD_INIT(0)
  0,                    /*ob_size*/
  "PyRTTypeObject",                    /*tp_name*/
  sizeof(PyRTTypeObject),      /*tp_basicsize*/
  0,                    /*tp_itemsize*/
  /* methods */
  (destructor)PyRTTypeObject_dealloc,
  0,                    /*tp_print*/
  (getattrfunc)PyRTTypeObject_getattr,
  0, //(setattrfunc)PyRTTypesObject_setattr,
  0,                    /*tp_compare/tp_reserved*/
  (reprfunc)PyRTTypeObject_repr,
  0,                    /*tp_as_number*/
  0,                    /*tp_as_sequence*/
  0,                    /*tp_as_mapping*/
  0,                    /*tp_hash*/
  (ternaryfunc)PyRTTypeObject_call,
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,        /*tp_flags*/
};

struct PyRTTypesObject
{
  PyObject_HEAD
  FEC_Ref fecContextRef;
};

static void PyRTTypesObject_dealloc( PyRTTypesObject *obj )
{
  FEC_RefRelease( obj->fecContextRef );
  // [pzion 20130730] This doesn't seem to be needed, even though it
  // is used in some examples??
  // PyMem_Del( obj );
}

static PyObject *PyRTTypesObject_getattr(
  PyRTTypesObject *pyRTTypesObject, char *name
  )
{
  if ( strcmp( name, "__members__" ) == 0 )
  {
    static const uint32_t maxNumNames = 256;
    char const *names[maxNumNames];
    uint32_t numNames;
    FEC_RTGetTypeNames(
      pyRTTypesObject->fecContextRef,
      maxNumNames,
      names,
      &numNames
      );
    if ( SetPyErrForFECException() )
      return 0;

    PyObject *resultPyObj = PyList_New( numNames );
    for ( uint32_t i=0; i<numNames; ++i )
    {
      PyList_SetItem(
        resultPyObj,
        i,
        PyString_FromString( names[i] )
        );
    }
    return resultPyObj;
  }

  PyRTTypeObject *pyRTTypeObject = reinterpret_cast<PyRTTypeObject *>(
    PyObject_CallObject(
      reinterpret_cast<PyObject *>( &PyRTTypeObjectType ),
      NULL
      )
    );
  if ( pyRTTypeObject )
  {
    pyRTTypeObject->fecContextRef = pyRTTypesObject->fecContextRef;
    FEC_RefRetain( pyRTTypeObject->fecContextRef );
    pyRTTypeObject->userNameCString = strdup( name );
  }
  return (PyObject *)pyRTTypeObject;
}

static PyObject *PyRTTypesObject_repr( PyRTTypesObject *pyRTTypesObject )
{
  return PyString_FromString( "<RT.types>" );
}

static PyTypeObject PyRTTypesObjectType =
{
  PyObject_HEAD_INIT(0)
  0,                    /*ob_size*/
  "PyRTTypesObject",                    /*tp_name*/
  sizeof(PyRTTypesObject),      /*tp_basicsize*/
  0,                    /*tp_itemsize*/
  /* methods */
  (destructor)PyRTTypesObject_dealloc,
  0,                    /*tp_print*/
  (getattrfunc)PyRTTypesObject_getattr,
  0, //(setattrfunc)PyRTTypesObject_setattr, /*tp_setattr*/
  0,                    /*tp_compare/tp_reserved*/
  (reprfunc)PyRTTypesObject_repr,
  0,                    /*tp_as_number*/
  0,                    /*tp_as_sequence*/
  0,                    /*tp_as_mapping*/
  0,                    /*tp_hash*/
  0, //(ternaryfunc)PyRTValObject_call,                    /*tp_call*/
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,        /*tp_flags*/
};

PyObject *CreatePyRTTypesObject( FabricCore::Context *context )
{
  PyRTTypesObject *pyObj = reinterpret_cast<PyRTTypesObject *>(
    PyObject_CallObject(
      reinterpret_cast<PyObject *>( &PyRTTypesObjectType ),
      NULL
      )
    );
  if ( pyObj )
  {
    pyObj->fecContextRef = context->getFECContextRef();
    FEC_RefRetain( pyObj->fecContextRef );
  }
  return (PyObject *)pyObj;
}

static PyObject *PyRTValObject_iter(PyObject *o)
{
  PyRTValObject *self = reinterpret_cast<PyRTValObject *>( o );
  if ( self )
  {
    if ( FEC_RTValIsArray( self->fecRTValRef ) )
    {
      if ( PyObject_GenericSetAttr( o,
                                    PyString_FromString( "__iterval__" ),
                                    PyInt_FromSize_t( 0 ) ) == -1 )
      {
        PyErr_Format( PyExc_RuntimeError, "unable to set iterator value" );
        return NULL;
      }
      Py_INCREF( o );
      return o;
    }
    else if ( FEC_RTValIsDict( self->fecRTValRef ) )
    {
      FEC_RTValRef rval = FEC_RTValGetDictKeys( self->fecRTValRef );

      if ( SetPyErrForFECException() )
        return NULL;

      PyObject *keysObj = FEC_RTValToPyObject( self->fecContextRef, rval );

      if ( PyObject_GenericSetAttr( o,
                                    PyString_FromString( "__iterkeys__" ),
                                    keysObj ) == -1 )
      {
        PyErr_Format( PyExc_RuntimeError, "unable to set iterator keys" );
        return NULL;
      }

      if ( PyObject_GenericSetAttr( o,
                                    PyString_FromString( "__iterval__" ),
                                    PyInt_FromSize_t( 0 ) ) == -1 )
      {
        PyErr_Format( PyExc_RuntimeError, "unable to set iterator value" );
        return NULL;
      }

      Py_INCREF( o );
      return o;
    }
  }

  PyErr_Format( PyExc_RuntimeError, "iterator unsupported for this type" );
  return NULL;
}

static PyObject *PyRTValObject_iternext(PyObject *o)
{
  PyRTValObject *self = reinterpret_cast<PyRTValObject *>( o );
  if ( self )
  {
    if ( FEC_RTValIsArray( self->fecRTValRef ) )
    {
      PyObject *iterval =
          PyObject_GenericGetAttr( o, PyString_FromString( "__iterval__" ) );
      if ( iterval && PyInt_Check( iterval ) )
      {
        long index = PyInt_AsLong( iterval );
        if ( FEC_RTValGetArraySize( self->fecRTValRef ) > index )
        {
          FEC_RTValRef rval =
              FEC_RTValGetArrayElementRef( self->fecRTValRef, index );

          if ( SetPyErrForFECException() )
            return NULL;

          if ( PyObject_GenericSetAttr( o,
                                        PyString_FromString( "__iterval__" ),
                                        PyInt_FromSize_t( index + 1 ) ) == -1 )
          {
            PyErr_Format( PyExc_RuntimeError, "unable to set iterator value" );
            return NULL;
          }

          return FEC_RTValToPyObject( self->fecContextRef, rval );
        }
        else
          return NULL;
      }
      PyErr_Format( PyExc_RuntimeError, "unable to read iterator value" );
      return NULL;
    }
    else if ( FEC_RTValIsDict( self->fecRTValRef ) )
    {
      PyObject *iterval =
          PyObject_GenericGetAttr( o, PyString_FromString( "__iterval__" ) );
      PyObject *iterkeys =
          PyObject_GenericGetAttr( o, PyString_FromString( "__iterkeys__" ) );
      if ( iterval && PyInt_Check( iterval ) && iterkeys )
      {

        FEC_RTValRef keysRef = PyObjectToFECRTValRef(
          self->fecContextRef,
          iterkeys
          );
        if ( !FEC_RTValIsArray( keysRef ) )
        {
          PyErr_Format( PyExc_RuntimeError, "unable to read iterator keys" );
          return NULL;
        }

        long index = PyInt_AsLong( iterval );
        if ( FEC_RTValGetArraySize( keysRef ) > index )
        {
          FEC_RTValRef keyRval =
              FEC_RTValGetArrayElementRef( keysRef, index );

          if ( SetPyErrForFECException() )
            return NULL;

          if ( PyObject_GenericSetAttr( o,
                                        PyString_FromString( "__iterval__" ),
                                        PyInt_FromSize_t( index + 1 ) ) == -1 )
          {
            PyErr_Format( PyExc_RuntimeError, "unable to set iterator value" );
            return NULL;
          }

          return FEC_RTValToPyObject( self->fecContextRef, keyRval );
        }
        else
          return NULL;
      }
      PyErr_Format( PyExc_RuntimeError, "unable to read iterator values" );
      return NULL;
    }
  }

  PyErr_Format(
    PyExc_RuntimeError,
    "iterator unsupported for this type"
    );
  return NULL;
}

class PyTypesInitializer
{
public:
  PyTypesInitializer()
  {
    PyRTTypesObjectType.tp_new = PyType_GenericNew;
    PyType_Ready( &PyRTTypesObjectType );

    PyRTValMethodObjectType.tp_new = PyType_GenericNew;
    PyType_Ready( &PyRTValMethodObjectType );

    PyRTValObjectType.tp_new = PyType_GenericNew;
    PyRTValObjectType.tp_iter = PyRTValObject_iter;
    PyRTValObjectType.tp_iternext = PyRTValObject_iternext;
    PyRTValObjectType.tp_dictoffset = offsetof( PyRTValObject, dict );
    PyType_Ready( &PyRTValObjectType );

    PyRTTypeMethodObjectType.tp_new = PyType_GenericNew;
    PyType_Ready( &PyRTTypeMethodObjectType );

    PyRTTypeObjectType.tp_new = PyType_GenericNew;
    PyType_Ready( &PyRTTypeObjectType );
  }
};
static PyTypesInitializer s_pyTypesInitializer;

FEC_RTValRef PyRTValObjectToRTValRef( PyObject *pyObject )
{
  if (Py_TYPE(pyObject) != &PyRTValObjectType)
  {
    PyErr_SetString(PyExc_ValueError, "item must be an RTVal");
    return FEC_NULL_REF;
  }

  PyRTValObject *pyRTValObject = (PyRTValObject *)pyObject;
  return pyRTValObject->fecRTValRef;
}

char *PyStringToCString( PyObject *pyString, char const *errorPrefix )
{
  char *result;
  if ( pyString == Py_None )
  {
    result = new char[1];
    result[0] = '\0';
  }
  else if ( PyString_Check( pyString ) )
  {
    Py_ssize_t size = PyString_Size( pyString );
    result = new char[size + 1];
    char const *pyStringAsString = PyString_AsString( pyString );
    if ( !pyStringAsString )
      return 0;
    memcpy( result, pyStringAsString, size );
    result[size] = '\0';
  }
  else if ( PyUnicode_Check( pyString ) )
  {
    PyObject *pyStringAsASCII =
      PyUnicode_AsASCIIString( pyString );
    Py_ssize_t size = PyString_Size( pyStringAsASCII );
    result = new char[size + 1];
    char const *pyStringAsASCIIAsString =
      PyString_AsString( pyStringAsASCII );
    if ( !pyStringAsASCIIAsString )
      return 0;
    memcpy( result, pyStringAsASCIIAsString, size );
    result[size] = '\0';
    Py_DECREF( pyStringAsASCII );
  }
  else
  {
    PyErr_Format(
      PyExc_ValueError,
      "%sexpecting a string, a unicode or None",
      errorPrefix
      );
    return NULL;
  }
  return result;
}
