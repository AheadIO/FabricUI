// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
# include <tchar.h>
#endif

#pragma warning(push, 0)
#include <QtGui/QtGui>
#include <FabricCore.h>
#include <FTL/JSONValue.h>
#pragma warning(pop)

#include <vector>
#include <assert.h>

#include "DllExport.h"
#include "QVariantRTVal.h"
