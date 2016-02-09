//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "LoadFabricStyleSheet.h"

#include <FTL/Path.h>
#include <QtCore/QFile>
#include <stdio.h>
#include <stdlib.h>
#include <string>

QString LoadFabricStyleSheet( FTL::StrRef basename )
{
  char const *fabricDir = getenv( "FABRIC_DIR" );
  if ( !fabricDir || !fabricDir[0] )
  {
    fprintf(
      stderr,
      "VETreeWidget::reloadStyles: missing FABRIC_DIR environment variable\n"
      );
    return QString();
  }

  std::string qssPath = fabricDir;
  FTL::PathAppendEntry( qssPath, "Resources" );
  FTL::PathAppendEntry( qssPath, "QSS" );
  FTL::PathAppendEntry( qssPath, basename );

#if defined(FTL_PLATFORM_WINDOWS)
  QFile qssFile( QString::fromLatin1( qssPath.data(), qssPath.size() ) );
#else
  QFile qssFile( QString::fromUtf8( qssPath.data(), qssPath.size() ) );
#endif
  if ( !qssFile.open( QFile::ReadOnly ) )
  {
    fprintf(
      stderr,
      "VETreeWidget::reloadStyles: unable to open '%s'\n",
      qssPath.c_str()
      );
    return QString();
  }

  QString styleSheet = QLatin1String( qssFile.readAll() );
  if ( styleSheet.isEmpty() )
  {
    fprintf(
      stderr,
      "VETreeWidget::reloadStyles: style sheet '%s' is empty\n",
      qssPath.c_str()
      );
  }
  return styleSheet;
}
