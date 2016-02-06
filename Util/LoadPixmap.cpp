//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "LoadPixmap.h"

#include <stdio.h>

namespace FabricUI {

//
// Load an pixmap from $FABRIC_DIR/Resources/Icons/{filename}.
// If the pixmap is not found, an error is printed and the resulting
// QPixmap has isNull() == true
//
QPixmap LoadPixmap( QString filename )
{
  char const *fabricDir = getenv( "FABRIC_DIR" );
  if ( !fabricDir || !fabricDir[0] )
  {
    fprintf(
      stderr,
      "LoadPixmap('%s'): FABRIC_DIR not set\n",
      filename.toUtf8().constData()
      );
    return QPixmap();
  }

  QString pathname = QString::fromUtf8( fabricDir );
  pathname += "/Resources/Icons/";
  pathname += filename;

  QPixmap pixmap( pathname );
  if ( pixmap.isNull() )
  {
    fprintf(
      stderr,
      "LoadPixmap('%s'): file '%s' does not exist or cannot be read\n",
      filename.toUtf8().constData(),
      pathname.toUtf8().constData()
      );
  }
  return pixmap;
}

}
