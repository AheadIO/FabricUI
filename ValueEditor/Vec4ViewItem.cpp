//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "QVariantRTVal.h"
#include "Vec4ViewItem.h"
#include "ViewItemFactory.h"
#include "VELineEdit.h"

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

  m_xEdit = new VELineEdit( QString::number( m_vec4dValue.x() ), m_widget );
  m_yEdit = new VELineEdit( QString::number( m_vec4dValue.y() ), m_widget );
  m_zEdit = new VELineEdit( QString::number( m_vec4dValue.z() ), m_widget );
  m_tEdit = new VELineEdit( QString::number( m_vec4dValue.w() ), m_widget );

  // Connect em up.
  connect(
    m_xEdit, SIGNAL(textModified( QString )),
    this, SLOT(onTextEditXModified( QString ))
    );
  connect(
    m_yEdit, SIGNAL(textModified( QString )),
    this, SLOT(onTextEditYModified( QString ))
    );
  connect(
    m_zEdit, SIGNAL(textModified( QString )),
    this, SLOT(onTextEditZModified( QString ))
    );
  connect(
    m_tEdit, SIGNAL(textModified( QString )),
    this, SLOT(onTextEditTModified( QString ))
    );

  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  layout->addWidget( m_xEdit );
  layout->addWidget( m_yEdit );
  layout->addWidget( m_zEdit );
  layout->addWidget( m_tEdit );
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
    m_xEdit->setText( QString::number( newVec4dValue.x() ) );
    routeModelValueChanged( 0, QVariant( newVec4dValue.x() ) );
  }
  if ( newVec4dValue.y() != m_vec4dValue.y() )
  {
    m_yEdit->setText( QString::number( newVec4dValue.y() ) );
    routeModelValueChanged( 1, QVariant( newVec4dValue.y() ) );
  }
  if ( newVec4dValue.z() != m_vec4dValue.z() )
  {
    m_zEdit->setText( QString::number( newVec4dValue.z() ) );
    routeModelValueChanged( 2, QVariant( newVec4dValue.z() ) );
  }
  if ( newVec4dValue.w() != m_vec4dValue.w() )
  {
    m_tEdit->setText( QString::number( newVec4dValue.w() ) );
    routeModelValueChanged( 3, QVariant( newVec4dValue.w() ) );
  }
  m_vec4dValue = newVec4dValue;
}

void Vec4ViewItem::onTextEditXModified( QString text )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setX( text.toDouble() );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onTextEditYModified( QString text )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setY( text.toDouble() );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onTextEditZModified( QString text )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setZ( text.toDouble() );
  emit viewValueChanged( QVariant( vec4d ) );
}

void Vec4ViewItem::onTextEditTModified( QString text )
{
  QVector4D vec4d = m_vec4dValue;
  vec4d.setW( text.toDouble() );
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
