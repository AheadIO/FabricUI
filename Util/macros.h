#ifndef __UI_UTIL_MACROS_H__
#define __UI_UTIL_MACROS_H__

#include <cctype>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#include <FTL/FS.h>
#include <FTL/Path.h>
#include <FTL/StrRef.h>
#include <FTL/CStrRef.h>
#include <FTL/JSONEnc.h>
#include <FTL/OwnedPtr.h>
#include <FTL/MapCharSingle.h>
#include <FabricCore.h>

#include <Core/Build.h>
#include <QtCore/QString>

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
#endif //_UI_UTIL_MACROS_H__
