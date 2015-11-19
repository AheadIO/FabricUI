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
      FTL::OwnedPtr<FTL::JSONObject const> &jsonObject,
      bool modal
      )
      : m_client( client )
      , m_jsonObject( jsonObject.take() )
      , m_modal( modal )
      {}

    void run()
    {
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
            onExpiryWarning( expiry );
            return;
          }
        }
      }
      else
      {
        onMainDialog();
        return;
      }

      onFinished();
    }

  protected slots:

    void onMainDialog()
    {
      QDialog *dialog = new MainLicenseDialog( 0, m_jsonObject.get() );
      if ( m_modal )
      {
        int result = dialog->exec();
        delete dialog;
        if ( result == QDialog::Accepted )
          onEnterLicense();
        else
          onContinueWithoutLicense();
      }
      else
      {
        connect(
          dialog, SIGNAL(accepted()),
          this, SLOT(onEnterLicense())
          );
        connect(
          dialog, SIGNAL(rejected()),
          this, SLOT(onContinueWithoutLicense())
          );
        dialog->setAttribute( Qt::WA_DeleteOnClose );
        dialog->show();
      }
    }

    void onExpiryWarning( FTL::CStrRef expiry )
    {
      const char *noExpiryDialog = getenv( "FABRIC_NO_EXPIRY_DIALOG" );
      if ( noExpiryDialog && ::atoi( noExpiryDialog ) > 0 )
      {
        fprintf( stderr,
                 "WARNING: Your Fabric license will expire in less than a "
                 "month on %s, please contact Fabric Software if you wish "
                 "to renew it.\n",
                 expiry.c_str() );
        return;
      }

      QDialog *dialog = new ExpiryWarningDialog( 0, expiry );
      if ( m_modal )
      {
        int result = dialog->exec();
        delete dialog;
        if ( result == QDialog::Accepted )
          onEnterLicense();
        else
          onFinished();
      }
      else
      {
        connect(
          dialog, SIGNAL(accepted()),
          this, SLOT(onEnterLicense())
          );
        connect(
          dialog, SIGNAL(rejected()),
          this, SLOT(onFinished())
          );
        dialog->setAttribute( Qt::WA_DeleteOnClose );
        dialog->show();
      }
    }

    void onEnterLicense()
    {
      EnterLicenseDialog *dialog = new EnterLicenseDialog( NULL );
      if ( m_modal )
      {
        int result = dialog->exec();
        QString licenseText = dialog->licenseText();
        delete dialog;
        if ( result == QDialog::Accepted )
          onLiceseEntered( licenseText );
        else
          onContinueWithoutLicense();
      }
      else
      {
        connect(
          dialog, SIGNAL(licenseEntered(QString)),
          this, SLOT(onLiceseEntered(QString))
          );
        connect(
          dialog, SIGNAL(rejected()),
          this, SLOT(onContinueWithoutLicense())
          );
        dialog->setAttribute( Qt::WA_DeleteOnClose );
        dialog->show();
      }
    }

    void onLiceseEntered( QString licenseText )
    {
      QByteArray text = licenseText.toUtf8();
      if ( !text.isEmpty() )
      {
        text.append( '\0' );
        FabricCore::SetStandaloneLicense( text.constData() );
      }
      if ( m_client.validateLicense() )
        onValidLicenseEntered();
      else
        onInvalidLicenseEntered();
    }

    void onValidLicenseEntered()
    {
      QDialog *dialog = new LicenseOkDialog( NULL );
      if ( m_modal )
      {
        dialog->exec();
        delete dialog;
        onFinished();
      }
      else
      {
        connect(
          dialog, SIGNAL(finished(int)),
          this, SLOT(onFinished())
          );
        dialog->setAttribute( Qt::WA_DeleteOnClose );
        dialog->show();
      }
    }

    void onInvalidLicenseEntered()
    {
      QDialog *dialog = new LicenseFailDialog( NULL );
      if ( m_modal )
      {
        dialog->exec();
        delete dialog;
        onEnterLicense();
      }
      else
      {
        connect(
          dialog, SIGNAL(finished(int)),
          this, SLOT(onEnterLicense())
          );
        dialog->setAttribute( Qt::WA_DeleteOnClose );
        dialog->show();
      }
    }

    void onContinueWithoutLicense()
    {
      LastWarningDialog *dialog = new LastWarningDialog( NULL );
      if ( m_modal )
      {
        dialog->exec();
        delete dialog;
        onFinished();
      }
      else
      {
        connect(
          dialog, SIGNAL(finished(int)),
          this, SLOT(onFinished())
          );
        dialog->setAttribute( Qt::WA_DeleteOnClose );
        dialog->show();
      }
    }

    void onFinished()
    {
      delete this;
    }

  private:

    FabricCore::Client m_client;
    FTL::OwnedPtr<FTL::JSONObject const> m_jsonObject;
    bool m_modal;
  };
}

#endif // __FABRICUI_LICENSE_DATA_HANDLER_H
