/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_LICENSING__
#define __FABRICUI_LICENSING__

#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <QtGui/QDialog>

namespace FabricUI
{
  void HandleLicenseData(
    QWidget *widgetParent,
    FabricCore::Client &client,
    FTL::StrRef jsonData,
    bool modalDialogs
    );
}

#endif // __FABRICUI_LICENSING__
