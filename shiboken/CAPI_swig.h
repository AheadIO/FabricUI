#ifndef CAPI_swig_h
#define CAPI_swig_h

#include <Python.h>

extern "C" {

struct swig_type_info;

#define SWIG_POINTER_OWN           0x1
#define SWIG_POINTER_NOSHADOW       (SWIG_POINTER_OWN      << 1)
#define SWIG_POINTER_NEW            (SWIG_POINTER_NOSHADOW | SWIG_POINTER_OWN)

#define SWIGTYPE_p_FEC_ClientCreateOptions swig_types[0]
#define SWIGTYPE_p_FEC_DFGCacheRule swig_types[1]
#define SWIGTYPE_p_FEC_DFGExecType swig_types[2]
#define SWIGTYPE_p_FEC_DFGNodeType swig_types[3]
#define SWIGTYPE_p_FEC_DFGPortType swig_types[4]
#define SWIGTYPE_p_FEC_KLSourceFile swig_types[5]
#define SWIGTYPE_p_FEC_KLTokenType swig_types[6]
#define SWIGTYPE_p_FEC_RTValSimpleData swig_types[7]
#define SWIGTYPE_p_FEC_RTValSimpleData_value swig_types[8]
#define SWIGTYPE_p_FEC_RTValSimpleData_value_string swig_types[9]
#define SWIGTYPE_p_FEC_RTValSimpleType swig_types[10]
#define SWIGTYPE_p_FEC_Variant swig_types[11]
#define SWIGTYPE_p_FEC_VariantDictIter swig_types[12]
#define SWIGTYPE_p_FabricCore__Client swig_types[13]
#define SWIGTYPE_p_FabricCore__Context swig_types[14]
#define SWIGTYPE_p_FabricCore__DFGBinding swig_types[15]
#define SWIGTYPE_p_FabricCore__DFGExec swig_types[16]
#define SWIGTYPE_p_FabricCore__DFGHost swig_types[17]
#define SWIGTYPE_p_FabricCore__DFGView swig_types[18]
#define SWIGTYPE_p_FabricCore__DGBinding swig_types[19]
#define SWIGTYPE_p_FabricCore__DGBindingList swig_types[20]
#define SWIGTYPE_p_FabricCore__DGCompiledObject swig_types[21]
#define SWIGTYPE_p_FabricCore__DGContainer swig_types[22]
#define SWIGTYPE_p_FabricCore__DGEvent swig_types[23]
#define SWIGTYPE_p_FabricCore__DGEventHandler swig_types[24]
#define SWIGTYPE_p_FabricCore__DGNamedObject swig_types[25]
#define SWIGTYPE_p_FabricCore__DGNode swig_types[26]
#define SWIGTYPE_p_FabricCore__DGOperator swig_types[27]
#define SWIGTYPE_p_FabricCore__Exception swig_types[28]
#define SWIGTYPE_p_FabricCore__KLTokenStream swig_types[29]
#define SWIGTYPE_p_FabricCore__RTVal swig_types[30]
#define SWIGTYPE_p_FabricCore__RTValFromJSONDecoder swig_types[31]
#define SWIGTYPE_p_FabricCore__RTValToJSONEncoder swig_types[32]
#define SWIGTYPE_p_FabricCore__Ref swig_types[33]
#define SWIGTYPE_p_FabricCore__String swig_types[34]
#define SWIGTYPE_p_FabricCore__Variant swig_types[35]
#define SWIGTYPE_p_PythonRTValFromJSONDecoder swig_types[36]
#define SWIGTYPE_p_PythonRTValToJSONEncoder swig_types[37]
#define SWIGTYPE_p_ReportCallback swig_types[38]
#define SWIGTYPE_p_SimpleData swig_types[39]
#define SWIGTYPE_p_SlowOperationCallback swig_types[40]
#define SWIGTYPE_p_StatusCallback swig_types[41]
#define SWIGTYPE_p___int64 swig_types[42]
#define SWIGTYPE_p_char swig_types[43]
#define SWIGTYPE_p_f_p_void_p_q_const__char_unsigned_int__void swig_types[44]
#define SWIGTYPE_p_f_p_void_p_q_const__char_unsigned_int_p_q_const__char_unsigned_int__void swig_types[45]
#define SWIGTYPE_p_f_p_void_p_void_p_void_unsigned_char_p_f_p_void_p_q_const__char_unsigned_int__void_p_void_p_f_p_void_p_q_const__char__p_q_const__char_p_void__unsigned_char swig_types[46]
#define SWIGTYPE_p_f_p_void_p_void_p_void_unsigned_char_p_q_const__char_unsigned_int_p_f_p_void_p_q_const__char__p_q_const__char_p_void__unsigned_char swig_types[47]
#define SWIGTYPE_p_f_p_void_unsigned_char_unsigned_char_p_q_const__char_unsigned_int__void swig_types[48]
#define SWIGTYPE_p_float swig_types[49]
#define SWIGTYPE_p_int swig_types[50]
#define SWIGTYPE_p_long swig_types[51]
#define SWIGTYPE_p_long_long swig_types[52]
#define SWIGTYPE_p_p_char swig_types[53]
#define SWIGTYPE_p_p_f_p_void_p_q_const__char_unsigned_int__void swig_types[54]
#define SWIGTYPE_p_p_unsigned_long swig_types[55]
#define SWIGTYPE_p_p_void swig_types[56]
#define SWIGTYPE_p_short swig_types[57]
#define SWIGTYPE_p_signed___int64 swig_types[58]
#define SWIGTYPE_p_signed_char swig_types[59]
#define SWIGTYPE_p_unsigned___int64 swig_types[60]
#define SWIGTYPE_p_unsigned_char swig_types[61]
#define SWIGTYPE_p_unsigned_int swig_types[62]
#define SWIGTYPE_p_unsigned_long swig_types[63]
#define SWIGTYPE_p_unsigned_long_long swig_types[64]
#define SWIGTYPE_p_unsigned_short swig_types[65]
#define SWIGTYPE_p_void swig_types[66]
extern struct swig_type_info *swig_types[68];

PyObject *SWIG_Python_NewPointerObj( void *ptr, struct swig_type_info *type,
                                     int flags );

#define SWIG_Python_ConvertPtr(obj, pptr, type, flags)  SWIG_Python_ConvertPtrAndOwn(obj, pptr, type, flags, 0)
int SWIG_Python_ConvertPtrAndOwn( PyObject *obj, void **ptr,
                                  struct swig_type_info *ty, int flags,
                                  int *own );

#define SWIG_as_voidptr(a) const_cast< void * >(static_cast< const void * >(a)) 
#define SWIG_IsOK(r) (r >= 0)

void init_CAPI(void);

}
 
#endif

