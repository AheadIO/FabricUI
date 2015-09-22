/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_LICENSE_DATA_HANDLER_H
#define __FABRICUI_LICENSE_DATA_HANDLER_H

#include <FabricCore.h>
#include <FTL/JSONValue.h>

#include "EnterLicenseDialog.h"
#include "ExpiryWarningDialog.h"
#include "LastWarningDialog.h"
#include "LicenseFailDialog.h"
#include "LicenseOkDialog.h"
#include "MainLicenseDialog.h"

#include <time.h>

#define EXPIRY_PRE_WARNING_SECONDS  30*24*3600

namespace FabricUI
{
  class LicenseDataHandler : public QObject
  {
    Q_OBJECT
    
  public:

    LicenseDataHandler(
      FabricCore::Client &client,
      FTL::OwnedPtr<FTL::JSONObject const> &jsonObject
      )
      : m_client( client )
      , m_jsonObject( jsonObject.take() )
      {}

    void run()
    {
      QDialog *dialog = NULL;

      bool valid = m_jsonObject->getBoolean( FTL_STR( "valid" ) );
      if ( valid )
      {
        static bool s_checkedExpiry = false;
        static const FTL::CStrRef s_months[12] = { "jan", "feb", "mar", "apr",
                                                   "may", "jun", "jul", "aug",
                                                   "sep", "oct", "nov", "dec" };

        if ( !s_checkedExpiry )
        {
          s_checkedExpiry = true;

          FTL::CStrRef expiry = m_jsonObject->getString( FTL_STR( "exp" ) );

          int day, year;
          char mon[4];
          sscanf( expiry.c_str(), "%d-%3s-%d", &day, mon, &year );

          struct tm expiry_tm;
          memset( &expiry_tm, 0, sizeof( struct tm ) );
          expiry_tm.tm_year = year - 1900;
          expiry_tm.tm_mday = day;
          for ( int i = 0; i < 12; i++ )
          {
            if ( s_months[i] == FTL::CStrRef( mon ) )
            {
              expiry_tm.tm_mon = i;
              break;
            }
          }

          if ( mktime( &expiry_tm ) - time( NULL ) < EXPIRY_PRE_WARNING_SECONDS )
          {
            dialog = new ExpiryWarningDialog( 0, expiry );
            connect(
              dialog, SIGNAL(accepted()),
              this, SLOT(onEnterLicense())
              );
            connect(
              dialog, SIGNAL(rejected()),
              this, SLOT(onFinished())
              );
          }
        }
      }
      else
      {
        dialog = new MainLicenseDialog( 0, m_jsonObject.get() );
        connect(
          dialog, SIGNAL(accepted()),
          this, SLOT(onEnterLicense())
          );
        connect(
          dialog, SIGNAL(rejected()),
          this, SLOT(onContinueWithoutLicense())
          );
      }

      if ( dialog )
      {
        dialog->setAttribute( Qt::WA_DeleteOnClose );
        dialog->show();
      }
    }

  protected slots:

    void onEnterLicense()
    {
      EnterLicenseDialog *dialog = new EnterLicenseDialog( NULL );
      connect(
        dialog, SIGNAL(licenseEntered(QString)),
        this, SLOT(onLiceseEntered(QString))
        );
      connect(
        dialog, SIGNAL(rejected()),
        this, SLOT(onFinish())
        );
      dialog->setAttribute( Qt::WA_DeleteOnClose );
      dialog->show();
    }

    void onLiceseEntered( QString licenseText )
    {
      std::string text = licenseText.toStdString();
      if ( !text.empty() )
        FabricCore::SetStandaloneLicense( text.c_str() );

      QDialog *dialog;
      if ( m_client.validateLicense() )
      {
        dialog = new LicenseOkDialog( NULL );
        connect(
          dialog, SIGNAL(accepted()),
          this, SLOT(onFinished())
          );
        connect(
          dialog, SIGNAL(rejected()),
          this, SLOT(onFinished())
          );
      }
      else
      {
        dialog = new LicenseFailDialog( NULL );
        connect(
          dialog, SIGNAL(accepted()),
          this, SLOT(onEnterLicense())
          );
        connect(
          dialog, SIGNAL(rejected()),
          this, SLOT(onEnterLicense())
          );
      }
      dialog->setAttribute( Qt::WA_DeleteOnClose );
      dialog->show();
    }

    void onContinueWithoutLicense()
    {
      LastWarningDialog *dialog = new LastWarningDialog( NULL );
      connect(
        dialog, SIGNAL(accepted()),
        this, SLOT(onFinished())
        );
      connect(
        dialog, SIGNAL(rejected()),
        this, SLOT(onFinished())
        );
      dialog->setAttribute( Qt::WA_DeleteOnClose );
      dialog->show();
    }

    void onFinished()
    {
      delete this;
    }

  private:

    FabricCore::Client m_client;
    FTL::OwnedPtr<FTL::JSONObject const> m_jsonObject;
  };
}

#endif // __FABRICUI_LICENSE_DATA_HANDLER_H
