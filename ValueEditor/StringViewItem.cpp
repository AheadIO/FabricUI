//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "StringViewItem.h"
#include "QVariantRTVal.h"

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QDialogButtonBox>

StringViewItem::StringViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem(name)
{
  m_widget = new QWidget;
  m_widget->setObjectName( "StringItem" );
  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  QPushButton* inspectButton = new QPushButton( m_widget );
  QIcon inspectIcon( "icons/mag_glass.png" );
  inspectButton->setIcon( inspectIcon );
  m_edit = new QLineEdit( m_widget );

  layout->addWidget( m_edit );
  layout->addWidget( inspectButton );

  connect(
    m_edit, SIGNAL( textEdited( const QString& ) ),
    this, SLOT( OnTextEdited( const QString& ) )
    );
  connect(
    m_edit, SIGNAL( editingFinished() ),
    this, SLOT( OnEditFinished() )
    );
  connect(
    inspectButton, SIGNAL( clicked() ),
    this, SLOT( onInspect() ) );
  onModelValueChanged( value );
}

StringViewItem::~StringViewItem()
{
}

QWidget *StringViewItem::getWidget()
{
  return m_widget;
}

void StringViewItem::onModelValueChanged( QVariant const &v )
{
  m_edit->setText( v.value<QString>() );
}

void StringViewItem::OnTextEdited( const QString& text )
{
  emit viewValueChanged(
    QVariant::fromValue<QString>( text )
    );
}

void StringViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue<QString>( m_edit->text() )
    );
}

void StringViewItem::onInspect()
{
  // Show a multi-line dialog to display 
  // the full string contents
  QDialog dlg( m_widget, Qt::Window | Qt::Dialog /*| Qt::FramelessWindowHint*/ );
  dlg.setModal( true );
  // Add text editor
  QVBoxLayout *layout = new QVBoxLayout( &dlg );
  QPlainTextEdit* txtEdit = new QPlainTextEdit;
  txtEdit->setPlainText( m_edit->text() );
  layout->addWidget( txtEdit );

  // Ok/cancel
  QDialogButtonBox* buttonBox = new QDialogButtonBox( 
                                      QDialogButtonBox::Ok
                                    | QDialogButtonBox::Cancel );

  connect( buttonBox, SIGNAL( accepted() ), &dlg, SLOT( accept() ) );
  connect( buttonBox, SIGNAL( rejected() ), &dlg, SLOT( reject() ) );
  layout->addWidget( buttonBox );

 
  // Post dialog under mouse
  QPoint pos = QCursor::pos();
  dlg.move( pos.x(), pos.y() );

  if (dlg.exec() == QDialog::Accepted)
  {
    OnTextEdited( txtEdit->toPlainText() );
  }
}

//////////////////////////////////////////////////////////////////////////
// 

BaseViewItem* StringViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if (RTVariant::isType<QString>(value))
  {
    return new StringViewItem( name, value );
  }
  return 0;
}

const int StringViewItem::Priority = 3;
