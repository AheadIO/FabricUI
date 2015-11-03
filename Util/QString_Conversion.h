// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_Util_QString_Conversion__
#define __UI_Util_QString_Conversion__

#define QSTRING_FROM_FTL_UTF8(s) QString::fromUtf8((s).data(), (s).size())
#define QSTRING_FROM_STL_UTF8(s) QString::fromUtf8((s).c_str(), (s).length())

#ifdef _WIN32
  #define QSTRING_TO_CONST_CHAR(s) (s).toAscii().constData()  // QString::toAscii() works fine under Windows, but not under Linux and OS X.
#else
  #define QSTRING_TO_CONST_CHAR(s) (s).toUtf8().constData()   // QString::toUtf8() works fine under Linux and OS X, but not under Windows.
#endif

#endif // __UI_Util_QString_Conversion__
