//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "QVariantRTVal.h"
#include "Vec3ViewItem.h"
#include "ViewItemFactory.h"

#include <assert.h>
#include <QtCore/QVariant>
#include <QtGui/QBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

Vec3ViewItem::Vec3ViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseComplexViewItem( name )
  , m_vec3dValue( value.value<QVector3D>() )
{
  m_widget = new QWidget;

  m_xEdit = new QLineEdit( QString::number( m_vec3dValue.x() ), m_widget );
  m_yEdit = new QLineEdit( QString::number( m_vec3dValue.y() ), m_widget );
  m_zEdit = new QLineEdit( QString::number( m_vec3dValue.z() ), m_widget );

  // Connect em up.
  connect(
    m_xEdit, SIGNAL(editingFinished()),
    this, SLOT(onTextEditXChanged())
    );
  connect(
    m_yEdit, SIGNAL(editingFinished()),
    this, SLOT(onTextEditYChanged())
    );
  connect(
    m_zEdit, SIGNAL(editingFinished()),
    this, SLOT(onTextEditZChanged())
    );

  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  layout->addWidget( m_xEdit );
  layout->addWidget( m_yEdit );
  layout->addWidget( m_zEdit );
}

Vec3ViewItem::~Vec3ViewItem()
{
}

QWidget *Vec3ViewItem::getWidget()
{
  return m_widget;
}

void Vec3ViewItem::onModelValueChanged( QVariant const &value )
{
  QVector3D newVec3dValue = value.value<QVector3D>();
  if ( newVec3dValue.x() != m_vec3dValue.x() )
  {
    m_xEdit->setText( QString::number( newVec3dValue.x() ) );
    routeModelValueChanged( 0, QVariant( newVec3dValue.x() ) );
  }
  if ( newVec3dValue.y() != m_vec3dValue.y() )
  {
    m_yEdit->setText( QString::number( newVec3dValue.y() ) );
    routeModelValueChanged( 1, QVariant( newVec3dValue.y() ) );
  }
  if ( newVec3dValue.z() != m_vec3dValue.z() )
  {
    m_zEdit->setText( QString::number( newVec3dValue.z() ) );
    routeModelValueChanged( 2, QVariant( newVec3dValue.z() ) );
  }
  m_vec3dValue = newVec3dValue;
}

void Vec3ViewItem::onTextEditXChanged()
{
  QVector3D vec3d = m_vec3dValue;
  vec3d.setX( m_xEdit->text().toDouble() );
  emit viewValueChanged( QVariant( vec3d ) );
}

void Vec3ViewItem::onTextEditYChanged()
{
  QVector3D vec3d = m_vec3dValue;
  vec3d.setY( m_yEdit->text().toDouble() );
  emit viewValueChanged( QVariant( vec3d ) );
}

void Vec3ViewItem::onTextEditZChanged()
{
  QVector3D vec3d = m_vec3dValue;
  vec3d.setZ( m_zEdit->text().toDouble() );
  emit viewValueChanged( QVariant( vec3d ) );
}

void Vec3ViewItem::onChildViewValueChanged(
  int index,
  QVariant value
  )
{
  QVector3D vec3d = m_vec3dValue;
  switch ( index )
  {
    case 0:
      vec3d.setX( value.toDouble() );
      break;
    case 1:
      vec3d.setY( value.toDouble() );
      break;
    case 2:
      vec3d.setZ( value.toDouble() );
      break;
    default:
      assert( false );
      break;
  }
  emit viewValueChanged( QVariant( vec3d ) );
}

void Vec3ViewItem::doAppendChildViewItems(QList<BaseViewItem *>& items)
{
  ViewItemFactory* factory = ViewItemFactory::GetInstance();

  BaseViewItem *children[3];
  children[0] = factory->CreateViewItem( "X", QVariant( m_vec3dValue.x() ), &m_metadata );
  children[1] = factory->CreateViewItem( "Y", QVariant( m_vec3dValue.y() ), &m_metadata );
  children[2] = factory->CreateViewItem( "Z", QVariant( m_vec3dValue.z() ), &m_metadata );
  for ( int i = 0; i < 3; ++i )
  {
    connectChild( i, children[i] );
    items.append( children[i] );
  }
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* Vec3ViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata*
  )
{
  if ( RTVariant::isType<QVector3D>( value ) )
    return new Vec3ViewItem( name, value );
  else
    return 0;
}

const int Vec3ViewItem::Priority = 3;
