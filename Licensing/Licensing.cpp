/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FTL/JSONValue.h>

#include "EnterLicenseDialog.h"
#include "LastWarningDialog.h"
#include "LicenseFailDialog.h"
#include "LicenseOkDialog.h"
#include "MainLicenseDialog.h"
#include "Licensing.h"

namespace FabricUI
{
  void HandleLicenseData( QWidget *widgetParent, FabricCore::Client &client, FTL::StrRef jsonData )
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonData );
    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>() );

    bool valid = jsonObject->getBoolean( FTL_STR( "valid" ) );
    if ( !valid )
    {
      MainLicenseDialog main( widgetParent, jsonObject.get() );
      if ( main.exec() == QDialog::Accepted )
      {
        while ( true )
        {
          EnterLicenseDialog enter( widgetParent );
          if ( enter.exec() == QDialog::Accepted )
          {
            std::string text = enter.getTextBox()->toPlainText().toStdString();
            if ( !text.empty() )
              FabricCore::SetStandaloneLicense( text.c_str() );
            if ( client.validateLicense() )
            {
              LicenseOkDialog ok( widgetParent );
              ok.exec();
              return;
            }
            else
            {
              LicenseFailDialog fail( widgetParent );
              fail.exec();
            }
          }
          else
            break;
        }
      }

      LastWarningDialog last( widgetParent );
      last.exec();
    }
  }
}
