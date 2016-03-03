//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "LoadFabricStyleSheet.h"

#include <FTL/Path.h>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <stdio.h>
#include <stdlib.h>
#include <string>

static QString ExpandEnvironmentVariables( QString string )
{
  static bool initialized = false;
  static QRegExp re;
  if ( !initialized )
  {
    initialized = true;
    re = QRegExp( "\\$\\{([^}]+)\\}" );
    re.setMinimal( true );
  }

  QString result;

  int offset = 0;
  for (;;)
  {
    int pos = re.indexIn( string, offset );
    if ( pos == -1 )
      break;

    if ( pos > offset )
      result += string.midRef( offset, pos - offset );

    QString envvarName = re.cap( 1 );
    char const *envvarValue = getenv( envvarName.toAscii().constData() );
    if ( envvarValue )
    {
#if defined(FTL_PLATFORM_WINDOWS)
      result += QString::fromLatin1( envvarValue );
#else
      result += QString::fromUtf8( envvarValue );
#endif
    }

    offset = pos + re.matchedLength();
  }

  result += string.midRef( offset );

  return result;
}

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
  return ExpandEnvironmentVariables( styleSheet );
}
