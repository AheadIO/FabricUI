#ifndef __MACROS_H__
#define __MACROS_H__

#include <FabricCore.h>

#ifdef FABRIC_SPLICE_TRACE

#define FABRIC_TRY(func, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return; \
  }

#define FABRIC_TRY_RETURN(func, returnVal, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return returnVal; \
  }

#define FABRIC_TRY_IGNORE(func, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
  }

#else

#define FABRIC_TRY(func, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return; \
  }

#define FABRIC_TRY_RETURN(func, returnVal, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return returnVal; \
  }

#define FABRIC_TRY_IGNORE(func, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
  }

#endif
#endif
