#include <Python.h>

#define SWIG_POINTER_OWN           0x1
#define SWIG_POINTER_NOSHADOW       (SWIG_POINTER_OWN      << 1)
#define SWIG_POINTER_NEW            (SWIG_POINTER_NOSHADOW | SWIG_POINTER_OWN)

#define SWIG_Python_ConvertPtr(obj, pptr, type, flags)  SWIG_Python_ConvertPtrAndOwn(obj, pptr, type, flags, 0)
#define SWIG_as_voidptr(a) const_cast< void * >(static_cast< const void * >(a)) 
#define SWIG_IsOK(r) (r >= 0)

extern "C" {
  struct swig_type_info;

  PyObject *SWIG_Python_NewPointerObj( void *ptr, struct swig_type_info *type,
                                       int flags );

  int SWIG_Python_ConvertPtrAndOwn( PyObject *obj, void **ptr,
                                    struct swig_type_info *ty, int flags,
                                    int *own );

  void init_CAPI(void);
}
 
