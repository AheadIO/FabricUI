// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_Util_QString_Conversion__
#define __UI_Util_QString_Conversion__

#define QSTRING_FROM_FTL_UTF8(s) QString::fromUtf8((s).data(), (s).size())
#define QSTRING_FROM_STL_UTF8(s) QString::fromUtf8((s).c_str(), (s).length())

#define QSTRING_TO_CONST_CHAR_VALUE(s) (s).toAscii().constData()

#ifdef _WIN32
  #define QSTRING_TO_CONST_CHAR_FILE(s) (s).toAscii().constData()
#else
  #define QSTRING_TO_CONST_CHAR_FILE(s) (s).toUtf8().constData()
#endif

#endif // __UI_Util_QString_Conversion__
