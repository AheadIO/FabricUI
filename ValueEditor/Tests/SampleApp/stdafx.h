// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
# include <tchar.h>
#endif
#include <assert.h>

#pragma warning(push, 0)
#include <QtGui/QtGui>
#include <FabricCore.h>
#pragma warning(pop)

#include <DllExport.h>
#include <BaseModelItem.h>
#include <BaseViewItem.h>

// TODO: reference additional headers your program requires here
