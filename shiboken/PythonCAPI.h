/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FABRIC_PYTHON_CAPI_H
#define _FABRIC_PYTHON_CAPI_H

PyObject *PyObjectToVariant(PyObject *obj, FabricCore::Variant *result);
PyObject *VariantToPyObject(FabricCore::Variant const *result);

PyObject *RTValToPyObject(
  FabricCore::Context const *context,
  FabricCore::RTVal const *rtVal
  );
PyObject *RTValToPyObject_DFGHost(
  FabricCore::DFGHost const *dfgHost,
  FabricCore::RTVal const *rtVal
  );
PyObject *RTValToPyObject_DFGExec(
  FabricCore::DFGExec const *dfgExec,
  FabricCore::RTVal const *rtVal
  );
PyObject *RTValToPyObject_DFGBinding(
  FabricCore::DFGBinding const *dfgBinding,
  FabricCore::RTVal const *rtVal
  );

FabricCore::RTVal PyObjectToRTVal(
  FabricCore::Context const *context,
  PyObject *pyObject
  );
FabricCore::RTVal PyObjectToRTVal_DFGHost(
  FabricCore::DFGHost const *dfgHost,
  PyObject *pyObject
  );
FabricCore::RTVal PyObjectToRTVal_DFGExec(
  FabricCore::DFGExec const *dfgExec,
  PyObject *pyObject
  );
FabricCore::RTVal PyObjectToRTVal_DFGBinding(
  FabricCore::DFGBinding const *dfgBinding,
  PyObject *pyObject
  );

void PythonSingleStringCallback(
  void *reportUserdata,
  char const *data,
  uint32_t length
  );

void PythonReportCallback(
  void *reportUserdata,
  FabricCore::ReportSource source,
  FabricCore::ReportLevel level,
  char const *lineData,
  uint32_t lineSize
  );

void PythonStatusCallback(
  void *userdata,
  char const *destinationData, uint32_t destinationLength,
  char const *payloadData, uint32_t payloadLength
  );

class PythonRTValToJSONEncoder : public FabricCore::RTValToJSONEncoder
{
public:

  PythonRTValToJSONEncoder( PyObject *pyRTValToJSONEncoder )
    : m_pyRTValToJSONEncoder( pyRTValToJSONEncoder )
  {
    Py_XINCREF( m_pyRTValToJSONEncoder );
  }

  virtual ~PythonRTValToJSONEncoder()
  {
    Py_XDECREF( m_pyRTValToJSONEncoder );
  }

  virtual FabricCore::RTValCodecResult encodeRTValToJSON(
    FabricCore::Context const &context,
    FabricCore::RTVal const &rtVal,
    FabricCore::RTValCodecContext codecContext,
    FabricCore::RTValToJSONEncoder::AppendFunctor const &append,
    FabricCore::RTValToJSONEncoder::MetadataLookupFunctor const &metadataLookup
    );

private:

  PyObject *m_pyRTValToJSONEncoder;
};

class PythonRTValFromJSONDecoder : public FabricCore::RTValFromJSONDecoder
{
public:

  PythonRTValFromJSONDecoder( PyObject *pyRTValFromJSONDecoder )
    : m_pyRTValFromJSONDecoder( pyRTValFromJSONDecoder )
  {
    Py_XINCREF( m_pyRTValFromJSONDecoder );
  }

  virtual ~PythonRTValFromJSONDecoder()
  {
    Py_XDECREF( m_pyRTValFromJSONDecoder );
  }

  virtual FabricCore::RTValCodecResult decodeRTValFromJSON(
    FabricCore::Context const &context,
    FabricCore::RTVal &rtVal,
    FabricCore::RTValCodecContext codecContext,
    char const *jsonData,
    uint32_t jsonSize,
    FabricCore::RTValFromJSONDecoder::MetadataLookupFunctor const &metadataLookup
    );

private:

  PyObject *m_pyRTValFromJSONDecoder;
};

PyObject *CreatePyRTTypesObject( FabricCore::Context *context );

FEC_RTValRef PyRTValObjectToRTValRef( PyObject *pyObject );

char *PyStringToCString( PyObject *pyString, char const *errorPrefix );

#endif //_FABRIC_PYTHON_CAPI_H
