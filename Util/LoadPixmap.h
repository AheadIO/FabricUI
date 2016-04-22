//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_UTIL_LOADPIXMAP_H
#define FABRICUI_UTIL_LOADPIXMAP_H

#include <QtGui/QPixmap>

namespace FabricUI {

//
// Load an pixmap from $FABRIC_DIR/Resources/Icons/{filename}.
// If the pixmap is not found, an error is printed and the resulting
// QPixmap has isNull() == true
//
QPixmap LoadPixmap( QString filename );

}

#endif // FABRICUI_UTIL_LOADPIXMAP_H
