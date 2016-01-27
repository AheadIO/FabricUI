//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "ColorViewItem.h"
#include "QVariantRTVal.h"
#include "ViewItemFactory.h"
#include "ItemMetadata.h"
#include "BaseModelItem.h"

#include <assert.h>
#include <FTL/JSONValue.h>
#include <QtCore/QVariant>
#include <QtGui/QColorDialog>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QComboBox>

#define IDX_RGB 0
#define IDX_HSV 1
#define META_FORMAT  "displayFormat"

ColorViewItem::ColorViewItem(
  const QVariant& value,
  const QString& name,
  ItemMetadata* metadata
  )
  : BaseComplexViewItem( name, metadata )
  , m_widget( NULL )
  , m_button( NULL )
  , m_spec( QColor::Rgb )
  , m_specCombo( NULL )
  , m_childMetadata (metadata)
{

  m_widget = new QWidget;
  m_widget->setObjectName( "ColorItem" );

  QHBoxLayout *layout = new QHBoxLayout( m_widget );

  m_button = new QPushButton();
  m_button->setAutoFillBackground( true );

  // Connect button signal to appropriate slot
  connect( m_button, SIGNAL( clicked() ),
           this, SLOT( pickColor() ) );

  layout->addWidget( m_button );

  m_specCombo = new QComboBox;
  m_specCombo->addItem( tr( "RGB" ) );
  m_specCombo->addItem( tr( "HSV" ) );

  connect( m_specCombo, SIGNAL( currentIndexChanged( const QString& ) ),
           this, SLOT( formatChanged( const QString& ) ) );

  layout->addWidget( m_specCombo );

  // Our children are inherently limited to 0->1
  m_childMetadata.setString( "uiRange", "(0.0, 1.0)" );

  metadataChanged();
  onModelValueChanged( value );
}

ColorViewItem::~ColorViewItem()
{
}

QWidget *ColorViewItem::getWidget()
{
  return m_widget;
}

void ColorViewItem::onModelValueChanged( QVariant const &value )
{
  m_color = value.value<QColor>();
  m_color = m_color.convertTo( m_spec );
  setButtonColor( m_color );

  switch (m_spec)
  {
    case QColor::Rgb:
      routeModelValueChanged( 0, QVariant( m_color.redF() ) );
      routeModelValueChanged( 1, QVariant( m_color.greenF() ) );
      routeModelValueChanged( 2, QVariant( m_color.blueF() ) );
      routeModelValueChanged( 3, QVariant( m_color.alphaF() ) );
      break;
    case QColor::Hsv:
      routeModelValueChanged( 0, QVariant( m_color.hueF() ) );
      routeModelValueChanged( 1, QVariant( m_color.saturationF() ) );
      routeModelValueChanged( 2, QVariant( m_color.valueF() ) );
      routeModelValueChanged( 3, QVariant( m_color.alphaF() ) );
      break;
    default:
      assert( !"Error: Bad Color Spec in ColorViewItem" );
      break;
  }
}

void ColorViewItem::onChildViewValueChanged( int index, QVariant value )
{
  QColor color = m_color;
  QColor::Spec spec = color.spec();
  switch (spec)
  {
    case QColor::Hsv:
      qreal v[4];
      color.getHsvF( v, v + 1, v + 2, v + 3 );
      v[index] = value.toDouble();
      color = QColor::fromHsvF( v[0], v[1], v[2], v[3] );
      break;

    case QColor::Rgb:
      switch (index)
      {
        case 0:
          color.setRedF( value.toDouble() );
          break;
        case 1:
          color.setGreenF( value.toDouble() );
          break;
        case 2:
          color.setBlueF( value.toDouble() );
          break;
        case 3:
          color.setAlphaF( value.toDouble() );
          break;
        default:
          assert( false );
          break;
      }
      break;

    default:
      assert( false );
      break;
  }
  emit viewValueChanged( QVariant( color ) );
}

void ColorViewItem::doAppendChildViewItems( QList<BaseViewItem*>& items )
{
  ViewItemFactory* factory = ViewItemFactory::GetInstance();
  BaseViewItem *children[4];
  switch (m_color.spec())
  {
    case QColor::Rgb:
      children[0] = factory->createViewItem( "R", QVariant( m_color.redF() ), &m_childMetadata );
      children[1] = factory->createViewItem( "G", QVariant( m_color.greenF() ), &m_childMetadata );
      children[2] = factory->createViewItem( "B", QVariant( m_color.blueF() ), &m_childMetadata );
      children[3] = factory->createViewItem( "A", QVariant( m_color.alphaF() ), &m_childMetadata );
      break;
    case QColor::Hsv:
      children[0] = factory->createViewItem( "H", QVariant( m_color.hueF() ), &m_childMetadata );
      children[1] = factory->createViewItem( "S", QVariant( m_color.saturationF() ), &m_childMetadata );
      children[2] = factory->createViewItem( "V", QVariant( m_color.valueF() ), &m_childMetadata );
      children[3] = factory->createViewItem( "A", QVariant( m_color.alphaF() ), &m_childMetadata );
      break;
    default:
      assert( !"Invalid Color" );
      return;
  }

  for (int i = 0; i < 4; ++i)
  {
    connectChild( i, children[i] );
    items.append( children[i] );
  }
}

void ColorViewItem::setButtonColor( const QColor& color ) 
{
  if (color.isValid())
  {
    // Set value via CSS to keep in sync with other styles
    QString qss = QString( "background-color: %1" ).arg( color.name() );
    m_button->setStyleSheet( qss );
  }
}

void ColorViewItem::metadataChanged()
{
  const char* dispType = m_metadata.getString( META_FORMAT );
  if (dispType == NULL)
    return;

  if (strcmp( dispType, "HSV" ) == 0)
  {
    m_spec = QColor::Hsv;
    m_specCombo->setCurrentIndex( IDX_HSV );
  }
  else
  {
    m_spec = QColor::Rgb;
    m_specCombo->setCurrentIndex( IDX_RGB );
  }
}

void ColorViewItem::pickColor()
{
  QColor currColor = m_button->palette().color( QPalette::Button );
  QColorDialog qcd( currColor, NULL );
  qcd.setOption( QColorDialog::ShowAlphaChannel, true );
  
  connect( &qcd, SIGNAL( colorSelected( QColor ) ), 
           this, SLOT( onColorSelected( QColor ) ) );
  connect( &qcd, SIGNAL( currentColorChanged( QColor ) ), 
           this, SLOT( onColorChanged( QColor ) ) );

  emit interactionBegin();
  
  qcd.exec();

  // If the user hits cancel, we wish to restore
  // the current state to the previous value
  emit interactionEnd( qcd.selectedColor().isValid() );
}

void ColorViewItem::onColorChanged( QColor color )
{
  emit viewValueChanged( QVariant::fromValue( color ) );
}

void ColorViewItem::onColorSelected( QColor color )
{
  emit viewValueChanged( QVariant::fromValue( color ) );
}

void ColorViewItem::formatChanged( const QString& format )
{
  BaseModelItem* modelItem = getModelItem();
  // Note: setting metadata will delete this
  // class
  if (format == tr("HSV"))
  {
    m_spec = QColor::Hsv;
    if (modelItem != NULL)
      modelItem->setMetadata( META_FORMAT, "HSV", 0 );
  }
  else
  {
    m_spec = QColor::Rgb;
    if (modelItem != NULL)
      modelItem->setMetadata( META_FORMAT, "RGB", 0 );
  }

  // Convert cached color
  onModelValueChanged( m_color );
  emit rebuildChildren( this );
}



//////////////////////////////////////////////////////////////////////////
//

BaseViewItem *ColorViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if (RTVariant::isType<QColor>(value))
  {
    return new ColorViewItem( value, name, metaData );
  }
  return 0;
}

const int ColorViewItem::Priority = 3;
