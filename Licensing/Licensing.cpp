/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#include <FTL/JSONValue.h>

#include "Licensing.h"
#include "LicenseDataHandler.h"

void FabricUI_HandleLicenseData(
  QWidget *widgetParent,
  FabricCore::Client &client,
  FTL::StrRef jsonData,
  bool modalDialogs
  )
{
  FTL::JSONStrWithLoc jsonStrWithLoc( jsonData );
  FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
    FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>() );

  FabricUI::LicenseDataHandler *licenseDataHandler =
    new FabricUI::LicenseDataHandler( client, jsonObject, modalDialogs );
  licenseDataHandler->run();
}


