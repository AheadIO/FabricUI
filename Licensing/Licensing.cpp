/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FTL/JSONValue.h>

#include "EnterLicenseDialog.h"
#include "ExpiryWarningDialog.h"
#include "LastWarningDialog.h"
#include "LicenseFailDialog.h"
#include "LicenseOkDialog.h"
#include "MainLicenseDialog.h"
#include "Licensing.h"

#include <time.h>

#define EXPIRY_PRE_WARNING_SECONDS  30*24*3600

namespace FabricUI
{
  void HandleLicenseData( QWidget *widgetParent, FabricCore::Client &client, FTL::StrRef jsonData )
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonData );
    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>() );

    bool valid = jsonObject->getBoolean( FTL_STR( "valid" ) );
    if ( valid )
    {
      static bool checkedExpiry = false;
      if ( !checkedExpiry )
      {
        checkedExpiry = true;
        FTL::CStrRef expiry = jsonObject->getString( FTL_STR( "exp" ) );

        struct tm expiry_tm;
        memset( &expiry_tm, 0, sizeof( struct tm ) );
        strptime( expiry.c_str(), "%d-%b-%Y", &expiry_tm );
        if ( mktime( &expiry_tm ) - time( NULL ) < EXPIRY_PRE_WARNING_SECONDS )
        {
          ExpiryWarningDialog expiryDialog( widgetParent, expiry );
          expiryDialog.exec();
        }
      }
    }
    else
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
