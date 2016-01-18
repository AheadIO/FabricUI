//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
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
  const QString& name
  )
  : BaseComplexViewItem( name )
  , m_widget( NULL )
  , m_button( NULL )
  , m_spec( QColor::Rgb )
  , m_specCombo( NULL )
{

  m_widget = new QWidget;
  m_widget->setObjectName( "ColorSwatch" );

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
      break;
    case QColor::Hsv:
      routeModelValueChanged( 0, QVariant( m_color.hueF() ) );
      routeModelValueChanged( 1, QVariant( m_color.saturationF() ) );
      routeModelValueChanged( 2, QVariant( m_color.valueF() ) );
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

  // Our children are inherently limited to 0->1
  ViewItemMetadata metadata;
  metadata.setFloat64( "min", 0.0 );
  metadata.setFloat64( "max", 1.0 );

  BaseViewItem *children[3];
  switch (m_color.spec())
  {
    case QColor::Rgb:
      children[0] = factory->CreateViewItem( "R", QVariant( m_color.redF() ), &metadata );
      children[1] = factory->CreateViewItem( "G", QVariant( m_color.greenF() ), &metadata );
      children[2] = factory->CreateViewItem( "B", QVariant( m_color.blueF() ), &metadata );
      break;
    case QColor::Hsv:
      children[0] = factory->CreateViewItem( "H", QVariant( m_color.hueF() ), &metadata );
      children[1] = factory->CreateViewItem( "S", QVariant( m_color.saturationF() ), &metadata );
      children[2] = factory->CreateViewItem( "V", QVariant( m_color.valueF() ), &metadata );
      break;
    default:
      assert( !"Invalid Color" );
      return;
  }

  for (int i = 0; i < 3; ++i)
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

void ColorViewItem::updateMetadata( ItemMetadata* metaData )
{
  BaseComplexViewItem::updateMetadata( metaData );
  const char* dispType = metaData->getString( META_FORMAT );
  if (dispType == NULL)
    return;

  if (strcmp( dispType, "HSV" ) == 0)
  {
    m_spec = QColor::Spec::Hsv;
    m_specCombo->setCurrentIndex( IDX_HSV );
  }
  else
  {
    m_spec = QColor::Spec::Rgb;
    m_specCombo->setCurrentIndex( IDX_RGB );
  }
}

void ColorViewItem::pickColor()
{
  QColor currColor = m_button->palette().color( QPalette::Button );
  QColorDialog qcd( currColor, NULL );
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
  BaseModelItem* modelItem = GetModelItem();
  if (format == tr("HSV"))
  {
    m_spec = QColor::Spec::Hsv;
    if (modelItem != NULL)
      modelItem->SetMetadata( META_FORMAT, "HSV", 0 );

  }
  else
  {
    m_spec = QColor::Spec::Rgb;
    if (modelItem != NULL)
      modelItem->SetMetadata( META_FORMAT, "RGB", 0 );
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
  ItemMetadata* /*metaData*/
  )
{
  if (RTVariant::isType<QColor>(value))
  {
    return new ColorViewItem( value, name );
  }
  return 0;
}

const int ColorViewItem::Priority = 3;
