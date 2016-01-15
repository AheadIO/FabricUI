//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "ColorViewItem.h"
#include "QVariantRTVal.h"
#include "ViewItemFactory.h"
#include "ItemMetadata.h"

#include <assert.h>
#include <FTL/JSONValue.h>
#include <QtCore/QVariant>
#include <QtGui/QColorDialog>
#include <QtGui/QPushButton>

ColorViewItem::ColorViewItem(
  const QVariant& value,
  const QString& name
  )
  : BaseComplexViewItem( name )
  , m_button( NULL )
{
  m_button = new QPushButton();
  m_button->setObjectName( name );
  m_button->setProperty( "class", QString( "colorSwatch" ) );
  m_button->setAutoFillBackground( true );

  // Connect button signal to appropriate slot
  connect( m_button, SIGNAL( clicked() ), this, SLOT( pickColor() ) );

  onModelValueChanged( value );
}

ColorViewItem::~ColorViewItem()
{
}

QWidget *ColorViewItem::getWidget()
{
  return m_button;
}

void ColorViewItem::onModelValueChanged( QVariant const &value )
{
  m_color = value.value<QColor>();
  setButtonColor( m_color );

  switch (m_color.spec())
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

void ColorViewItem::pickColor()
{
  QColor currColor = m_button->palette().color( QPalette::Button );
  QColorDialog qcd( currColor, NULL );
  connect( &qcd, SIGNAL( colorSelected( QColor ) ), 
           this, SLOT( onColorSelected( QColor ) ) );
  connect( &qcd, SIGNAL( currentColorChanged( QColor ) ), 
           this, SLOT( onColorChanged( QColor ) ) );
  qcd.exec();

  // If the user hits cancel, we wish to restore
  // the current state to the previous value
  QColor outColor = qcd.selectedColor();
  if (!outColor.isValid())
  {
    onColorChanged( currColor );
  }
}

void ColorViewItem::onColorChanged( QColor color )
{
  emit viewValueChanged( QVariant::fromValue( color ) );
}

void ColorViewItem::onColorSelected( QColor color )
{
  emit viewValueChanged( QVariant::fromValue( color ) );
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
