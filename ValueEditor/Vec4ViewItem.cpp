//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "QVariantRTVal.h"
#include "Vec4ViewItem.h"
#include "ViewItemFactory.h"
#include "VESpinBox.h"

#include <assert.h>
#include <QtCore/QVariant>
#include <QtGui/QBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

Vec4ViewItem::Vec4ViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseComplexViewItem( name )
  , m_vec4dValue( value.value<QVector4D>() )
{
  m_widget = new QWidget;

  m_xSpinBox = new VESpinBox( m_vec4dValue.x(), m_widget );
  m_ySpinBox = new VESpinBox( m_vec4dValue.y(), m_widget );
  m_zSpinBox = new VESpinBox( m_vec4dValue.z(), m_widget );
  m_tSpinBox = new VESpinBox( m_vec4dValue.w(), m_widget );

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
    m_xSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onXSpinBoxValueChanged(double))
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
    m_ySpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onYSpinBoxValueChanged(double))
    );
  
  connect(
    m_zSpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_zSpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_zSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onZSpinBoxValueChanged(double))
    );
  
  connect(
    m_tSpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_tSpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_tSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onTSpinBoxValueChanged(double))
    );

  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 8 );
  layout->addWidget( m_xSpinBox );
  layout->addWidget( m_ySpinBox );
  layout->addWidget( m_zSpinBox );
  layout->addWidget( m_tSpinBox );
}

Vec4ViewItem::~Vec4ViewItem()
{
}

QWidget *Vec4ViewItem::getWidget()
{
  return m_widget;
}

void Vec4ViewItem::onModelValueChanged( QVariant const &value )
{
  QVector4D newVec4dValue = value.value<QVector4D>();
  if ( newVec4dValue.x() != m_vec4dValue.x() )
  {
    m_xSpinBox->setValue( newVec4dValue.x() );
    routeModelValueChanged( 0, QVariant( newVec4dValue.x() ) );
  }
  if ( newVec4dValue.y() != m_vec4dValue.y() )
  {
    m_ySpinBox->setValue( newVec4dValue.y() );
    routeModelValueChanged( 1, QVariant( newVec4dValue.y() ) );
  }
  if ( newVec4dValue.z() != m_vec4dValue.z() )
  {
    m_zSpinBox->setValue( newVec4dValue.z() );
    routeModelValueChanged( 2, QVariant( newVec4dValue.z() ) );
  }
  if ( newVec4dValue.w() != m_vec4dValue.w() )
  {
    m_tSpinBox->setValue( newVec4dValue.w() );
    routeModelValueChanged( 3, QVariant( newVec4dValue.w() ) );
  }
  m_vec4dValue = newVec4dValue;
}

void Vec4ViewItem::onXSpinBoxValueChanged( double value )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setX( value );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onYSpinBoxValueChanged( double value )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setY( value );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onZSpinBoxValueChanged( double value )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setZ( value );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onTSpinBoxValueChanged( double value )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setW( value );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onChildViewValueChanged(
  int index,
  QVariant value
  )
{
  QVector4D vec4d = m_vec4dValue;
  switch ( index )
  {
    case 0:
      vec4d.setX( value.toDouble() );
      break;
    case 1:
      vec4d.setY( value.toDouble() );
      break;
    case 2:
      vec4d.setZ( value.toDouble() );
      break;
    case 3:
      vec4d.setW( value.toDouble() );
      break;
    default:
      assert( false );
      break;
  }
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::doAppendChildViewItems(QList<BaseViewItem *>& items)
{
  ViewItemFactory* factory = ViewItemFactory::GetInstance();

  BaseViewItem *children[4];
  children[0] = factory->CreateViewItem( "X", QVariant( m_vec4dValue.x() ), &m_metadata );
  children[1] = factory->CreateViewItem( "Y", QVariant( m_vec4dValue.y() ), &m_metadata );
  children[2] = factory->CreateViewItem( "Z", QVariant( m_vec4dValue.z() ), &m_metadata );
  children[3] = factory->CreateViewItem( "T", QVariant( m_vec4dValue.w() ), &m_metadata );
  for ( int i = 0; i < 4; ++i )
  {
    connectChild( i, children[i] );
    items.append( children[i] );
  }
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* Vec4ViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata*
  )
{
  if ( RTVariant::isType<QVector4D>( value ) )
    return new Vec4ViewItem( name, value );
  else
    return 0;
}

const int Vec4ViewItem::Priority = 3;
