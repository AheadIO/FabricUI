/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_LICENSING_EXPIRYWARNINGDIALOG_H
#define __FABRICUI_LICENSING_EXPIRYWARNINGDIALOG_H

#include <QtGui/QCursor>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

#include <FTL/CStrRef.h>

class ExpiryWarningDialog : public QDialog
{
  Q_OBJECT

public:

  ExpiryWarningDialog( QWidget *parent, FTL::CStrRef expiryDate )
    : QDialog( parent )
  {
    setWindowTitle( "Fabric Licensing" );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

    setLayout( new QVBoxLayout() );

    QLabel *info = new QLabel( this );
    info->setText("Your Fabric license will expire in less than a month on " +
                  QString( expiryDate.c_str() ) +
                  ",\nplease contact Fabric Software if you wish to renew it.");
    info->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    layout()->addWidget( info );

    QPushButton *enterNewButton = new QPushButton( "Enter New", this );
    QPushButton *continueButton = new QPushButton( "Continue", this );

    QObject::connect( enterNewButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    QObject::connect( continueButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    layout()->addWidget( enterNewButton );
    layout()->addWidget( continueButton );
  }
};

#endif // __FABRICUI_LICENSING_EXPIRYWARNINGDIALOG_H
