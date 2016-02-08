//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "QVariantRTVal.h"
#include "Vec2ViewItem.h"
#include "ViewItemFactory.h"
#include "VEDoubleSpinBox.h"
#include "ViewConstants.h"

#include <assert.h>
#include <FabricUI/Util/UIRange.h>
#include <QtCore/QVariant>
#include <QtGui/QBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

Vec2ViewItem::Vec2ViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseComplexViewItem( name, metadata )
  , m_vec2dValue( value.value<QVector2D>() )
{
  m_widget = new QWidget;
  m_widget->setObjectName( "Vec2Item" );

  m_xSpinBox = new VEDoubleSpinBox;
  m_xSpinBox->setValue( m_vec2dValue.x() );
  m_ySpinBox = new VEDoubleSpinBox;
  m_ySpinBox->setValue( m_vec2dValue.y() );

  // Connect em up.

  connect(
    m_xSpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_xSpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_xSpinBox, SIGNAL(valueChanged( double )),
    this, SLOT(onXSpinBoxValueChanged( double ))
    );

  connect(
    m_ySpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_ySpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_ySpinBox, SIGNAL(valueChanged( double )),
    this, SLOT(onYSpinBoxValueChanged( double ))
    );

  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 8 );
  layout->addWidget( m_xSpinBox );
  layout->addWidget( m_ySpinBox );

  metadataChanged();
}

Vec2ViewItem::~Vec2ViewItem()
{
}

QWidget *Vec2ViewItem::getWidget()
{
  return m_widget;
}

void Vec2ViewItem::onModelValueChanged( QVariant const &value )
{
  QVector2D newVec2dValue = value.value<QVector2D>();
  if ( newVec2dValue.x() != m_vec2dValue.x() )
  {
    m_xSpinBox->setValue( newVec2dValue.x() );
    routeModelValueChanged( 0, QVariant( newVec2dValue.x() ) );
  }
  if ( newVec2dValue.y() != m_vec2dValue.y() )
  {
    m_ySpinBox->setValue( newVec2dValue.y() );
    routeModelValueChanged( 1, QVariant( newVec2dValue.y() ) );
  }
  m_vec2dValue = newVec2dValue;
}

void Vec2ViewItem::onXSpinBoxValueChanged( double value )
{
  QVector2D vec2d = m_vec2dValue;
  vec2d.setX( value );
  emit viewValueChanged( QVariant( vec2d ) );
}

void Vec2ViewItem::onYSpinBoxValueChanged( double value )
{
  QVector2D vec2d = m_vec2dValue;
  vec2d.setY( value );
  emit viewValueChanged( QVariant( vec2d ) );
}

void Vec2ViewItem::onChildViewValueChanged(
  int index,
  QVariant value
  )
{
  QVector2D vec2d = m_vec2dValue;
  switch ( index )
  {
    case 0:
      vec2d.setX( value.toDouble() );
      break;
    case 1:
      vec2d.setY( value.toDouble() );
      break;
    default:
      assert( false );
      break;
  }
  emit viewValueChanged( QVariant( vec2d ) );
}

void Vec2ViewItem::doAppendChildViewItems(QList<BaseViewItem *>& items)
{
  ViewItemFactory* factory = ViewItemFactory::GetInstance();

  BaseViewItem *children[2];
  children[0] = factory->createViewItem( "X", QVariant( m_vec2dValue.x() ), &m_metadata );
  children[1] = factory->createViewItem( "Y", QVariant( m_vec2dValue.y() ), &m_metadata );
  for ( int i = 0; i < 2; ++i )
  {
    connectChild( i, children[i] );
    items.append( children[i] );
  }
}

void Vec2ViewItem::metadataChanged()
{
  FTL::StrRef uiRangeString = m_metadata.getString( "uiRange" );
  
  double minValue, maxValue;
  if ( FabricUI::DecodeUIRange( uiRangeString, minValue, maxValue ) )
  {
    m_xSpinBox->setRange( minValue, maxValue );
    m_ySpinBox->setRange( minValue, maxValue );
  }
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* Vec2ViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
{
  if ( RTVariant::isType<QVector2D>( value ) )
    return new Vec2ViewItem( name, value, metadata );
  else
    return 0;
}

const int Vec2ViewItem::Priority = 3;
