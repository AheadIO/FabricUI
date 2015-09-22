/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_LICENSING_MAINLICENSEDIALOG_H
#define __FABRICUI_LICENSING_MAINLICENSEDIALOG_H

#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QTextBrowser>
#include <QtGui/QCursor>
#include <QtGui/QDialog>

#include <FTL/JSONValue.h>

class MainLicenseDialog : public QDialog
{
  Q_OBJECT

public:
  MainLicenseDialog( QWidget *parent, FTL::JSONObject const *json )
    : QDialog( parent )
  {
    // setModal( true );
    setWindowModality( Qt::ApplicationModal );
    setWindowTitle( "Fabric Licensing" );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    setMinimumSize( 500, 350 );

    setLayout( new QVBoxLayout() );

    QTextBrowser *info = new QTextBrowser( this );
    info->setReadOnly( true );
    info->setFrameStyle( QFrame::NoFrame );
    info->setOpenExternalLinks( true );
    info->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    layout()->addWidget( info );

    QTextCursor infoCursor = info->textCursor();
    infoCursor.insertText( "This copy of Fabric Engine is currently unlicensed (" );
    infoCursor.insertText( json->getString( FTL_STR( "error" ) ).c_str() );
    infoCursor.insertText( ").\n\n" );

    infoCursor.insertText( "If you are a commercial customer or on Fabric 50 "
                           "please contact your company IT department or "
                           "license administrator.\n\n" );

    infoCursor.insertText( "If you are an individual user you can either enter "
                           "a node-locked license or continue unlicensed. An "
                           "unlicensed copy of Fabric will pause every 15 "
                           "minutes and display this popup.\n\n" );

    infoCursor.insertText( "Your MAC address(es) are:\n" );

    FTL::JSONArray const *hostIdsArray =
      json->get( FTL_STR( "hostIds" ) )->cast<FTL::JSONArray>();
    for ( size_t i = 0; i < hostIdsArray->size(); ++i )
    {
      FTL::JSONValue const *hostId = hostIdsArray->get( i );
      infoCursor.insertText( hostId->getStringValue().c_str() );
      infoCursor.insertText( "\n" );
    }

    QPushButton *enterLicenseButton = new QPushButton( "Enter Node-Locked License", this );
    QPushButton *continueButton = new QPushButton( "Continue Unlicensed", this );

    QObject::connect( continueButton, SIGNAL( clicked() ), this,
                      SLOT( reject() ) );
    QObject::connect( enterLicenseButton, SIGNAL( clicked() ), this, SLOT( accept() ) );

    layout()->addWidget( enterLicenseButton );
    layout()->addWidget( continueButton );
  }
};

#endif // __FABRICUI_LICENSING_MAINLICENSEDIALOG_H
