//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtGui/QPixmap>

namespace FabricUI {

//
// Load an pixmap from $FABRIC_DIR/Resources/Icons/{filename}.
// If the pixmap is not found, an error is printed and the resulting
// QPixmap has isNull() == true
//
QPixmap LoadPixmap( QString filename );

}
