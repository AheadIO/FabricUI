//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

// Must come first!! 
#include <FTL/Config.h>
#if defined(FTL_OS_DARWIN)
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <FabricUI/Util/GetFilenameForFileURL.h>

namespace FabricUI {
namespace Util {

QString GetFilenameForFileURL( QUrl url )
{
  QString scheme = url.scheme();
  if ( scheme == "file" )
  {
    QString filename = url.toLocalFile();
#if defined(FTL_OS_DARWIN)
    // [pzion 20150805] Work around
    // https://bugreports.qt.io/browse/QTBUG-40449
    if ( filename.startsWith("/.file/id=") )
    {
      CFStringRef relCFStringRef =
        CFStringCreateWithCString(
          kCFAllocatorDefault,
          filename.toUtf8().constData(),
          kCFStringEncodingUTF8
          );
      CFURLRef relCFURL =
        CFURLCreateWithFileSystemPath(
          kCFAllocatorDefault,
          relCFStringRef,
          kCFURLPOSIXPathStyle,
          false // isDirectory
          );
      CFErrorRef error = 0;
      CFURLRef absCFURL =
        CFURLCreateFilePathURL(
          kCFAllocatorDefault,
          relCFURL,
          &error
          );
      if ( !error )
      {
        static const CFIndex maxAbsPathCStrBufLen = 4096;
        char absPathCStr[maxAbsPathCStrBufLen];
        if ( CFURLGetFileSystemRepresentation(
          absCFURL,
          true, // resolveAgainstBase
          reinterpret_cast<UInt8 *>( &absPathCStr[0] ),
          maxAbsPathCStrBufLen
          ) )
        {
          filename = QString( absPathCStr );
        }
      }
      CFRelease( absCFURL );
      CFRelease( relCFURL );
      CFRelease( relCFStringRef );
    }
#endif // FTL_OS_DARWIN
    return filename;
  }
  else return QString();
}

} // namespace Util
} // namespace FabricUI
