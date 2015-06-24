/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_LICENSING_LICENSEOKDIALOG_H
#define __FABRICUI_LICENSING_LICENSEOKDIALOG_H

#include <QtGui/QCursor>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

class LicenseOkDialog : public QDialog
{
  Q_OBJECT

public:
  LicenseOkDialog( QWidget *parent )
    : QDialog( parent )
  {
    setModal( true );
    setWindowTitle( "Fabric Licensing" );
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    setLayout( new QVBoxLayout() );

    QLabel *info = new QLabel( this );
    info->setText( "License validation successful!" );
    info->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    layout()->addWidget( info );

    QPushButton *continueButton = new QPushButton( "Continue", this );

    QObject::connect( continueButton, SIGNAL( clicked() ), this, SLOT( accept() ) );

    layout()->addWidget( continueButton );
  }
};

#endif // __FABRICUI_LICENSING_LICENSEOKDIALOG_H
