/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FTL/JSONValue.h>

#include "Licensing.h"
#include "LicenseDataHandler.h"

namespace FabricUI
{
  void HandleLicenseData(
    QWidget *widgetParent,
    FabricCore::Client &client,
    FTL::StrRef jsonData,
    bool modalDialogs
    )
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonData );
    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>() );

    LicenseDataHandler *licenseDataHandler =
      new LicenseDataHandler( client, jsonObject, modalDialogs );
    licenseDataHandler->run();
  }
}
