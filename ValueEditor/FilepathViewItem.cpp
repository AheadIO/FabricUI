//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "FilepathViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"
#include "VELineEdit.h"

#include <QtCore/QVariant>
#include <QtGui/QWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QFileDialog>

using namespace FabricUI::ValueEditor;

inline QString ToQString( const QVariant& var );
FabricCore::RTVal ToFilePath( FabricCore::RTVal& val, const QString& text );

FilepathViewItem::FilepathViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
  , m_val(value.value<FabricCore::RTVal>())
{
  m_widget = new QWidget;
  m_widget->setObjectName( "FilePathItem" );
  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  QPushButton* browseButton = new QPushButton( m_widget );
  browseButton->setText( "..." );
  m_edit = new VELineEdit( m_widget );

  onModelValueChanged( value );

  layout->addWidget( m_edit );
  layout->addWidget( browseButton );

  connect(
    m_edit, SIGNAL( textModified( QString ) ),
    this, SLOT( onTextModified( QString ) )
    );
  connect( 
    browseButton, SIGNAL( clicked() ), 
    this, SLOT( doBrowse() ) );

  metadataChanged();
}

FilepathViewItem::~FilepathViewItem()
{
}

QWidget *FilepathViewItem::getWidget()
{
  return m_widget;
}

void FilepathViewItem::metadataChanged()
{
  if (m_metadata.has( "filter" ))
    m_filter = m_metadata.getString( "filter" );
  else
    m_filter = QString();
}

void FilepathViewItem::onModelValueChanged( QVariant const &v )
{
  m_edit->setText( ToQString(v) );
}

void FilepathViewItem::doBrowse()
{
  QString fileName = QFileDialog::getOpenFileName( m_widget,
                                           tr( "Open File" ), 
                                           QString(), 
                                           m_filter);
  if (!fileName.isEmpty())
  {
    // Update the string widget
    m_edit->setText( fileName );
    // Update the model
    onTextModified( fileName );
  }
}

void FilepathViewItem::onTextModified( QString text )
{
  m_val = ToFilePath( m_val, text );
  emit viewValueChanged(
    QVariant::fromValue<FabricCore::RTVal>( m_val )
    );
}

//////////////////////////////////////////////////////////////////////////
//

inline QString ToQString( const QVariant& var )
{
  if (var.isNull())
    return "";
  FabricCore::RTVal val = var.value<FabricCore::RTVal>();
  const FabricCore::RTVal pathStr = val.callMethod( "String", "string", 0, NULL );
  const char* asCStr = pathStr.getStringCString();
  return asCStr;
}

FabricCore::RTVal ToFilePath( FabricCore::RTVal& val, const QString& text )
{
  FabricCore::RTVal asString = FabricCore::RTVal::ConstructString(
    val.getContext(),
    text.toAscii().data() );

  return FabricCore::RTVal::Create(
    val.getContext(),
    "FilePath",
    1,
    &asString );
}


//////////////////////////////////////////////////////////////////////////
// 

BaseViewItem *FilepathViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if (isRTVal(value))
  {
    const FabricCore::RTVal& val = value.value<FabricCore::RTVal>( );
    if (val.isValid())
    {
      const char* rtype = val.getTypeNameCStr();
      if (strcmp( rtype, "FilePath" ) == 0)
        return new FilepathViewItem( name, value, metaData );
    }
  }
  return NULL;
}

const int FilepathViewItem::Priority = 3;
