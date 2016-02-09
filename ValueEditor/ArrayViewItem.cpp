//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "ArrayViewItem.h"
#include "ViewItemFactory.h"
#include "QVariantRTVal.h"
#include "VEIntSpinBox.h"
#include <QtGui/QWidget>
#include <QtGui/QBoxLayout>
#include <QtGui/QLabel>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

ArrayViewItem::ArrayViewItem( QString name,
                              const FabricCore::RTVal& value,
                              ItemMetadata* metadata )
  : BaseComplexViewItem( name, metadata )
  , m_val( value )
  , m_min( 0 )
  , m_max( 0 )
  , m_widget( NULL )
  , m_minIndexEdit( NULL )
  , m_maxIndexEdit( NULL )
  , m_arraySizeEdit( NULL )
{
  m_widget = new QWidget();

  if (m_val.isValid())
  {
    int arraySize = m_val.getArraySize();
    // By default don't display more than 100 elements
    m_max = min( arraySize, 100 );
  }
  m_minIndexEdit = new VEIntSpinBox( );
  m_maxIndexEdit = new VEIntSpinBox( );
  m_arraySizeEdit = new VEIntSpinBox( );

  updateWidgets();

  connect(
    m_minIndexEdit, SIGNAL( valueChanged( int ) ),
    this, SLOT( onMinIndexChanged( int ) )
    );
  connect(
    m_maxIndexEdit, SIGNAL( valueChanged( int ) ),
    this, SLOT( onMaxIndexChanged( int ) )
    );
  connect(
    m_arraySizeEdit, SIGNAL( valueChanged( int ) ),
    this, SLOT( onArraySizeChanged( int ) )
    );

  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  layout->addWidget( m_minIndexEdit );
  layout->addWidget( new QLabel( "to" ) );
  layout->addWidget( m_maxIndexEdit );
  layout->addWidget( new QLabel( "of" ) );
  layout->addWidget( m_arraySizeEdit );
}

ArrayViewItem::~ArrayViewItem()
{

}

void ArrayViewItem::doAppendChildViewItems( QList<BaseViewItem *>& items )
{
  try
  {
    // Construct a child for each instance between min & max
    ViewItemFactory* factory = ViewItemFactory::GetInstance();
    char childName[64];
    for (int i = m_min; i < m_max; ++i)
    {
      snprintf( childName, 64, "[%d]", i );

      FabricCore::RTVal childVal = m_val.getArrayElementRef( i );
      BaseViewItem* childItem =
        factory->createViewItem(
          childName,
          toVariant( childVal ),
          &m_metadata
          );

      if (childItem != NULL)
      {
        connectChild( i - m_min, childItem );
        items.push_back( childItem );
      }
    }
  }
  catch (FabricCore::Exception e)
  {
    const char* error = e.getDesc_cstr();
    printf( "%s", error );
  }
}

void ArrayViewItem::onChildViewValueChanged( int index, QVariant value )
{
  if (m_val.isValid() && m_val.isArray())
  {
    // We cannot simply create a new RTVal based on the QVariant type, as 
    // we have to set the type exactly the same as the original.  Get the
    // original child value to ensure the new value matches the internal type
    int arrayIndex = index + m_min;
    FabricCore::RTVal oldChildVal = m_val.getArrayElementRef( arrayIndex );
    RTVariant::toRTVal( value, oldChildVal );
    m_val.setArrayElement( arrayIndex, oldChildVal );
  }
  emit viewValueChanged( toVariant( m_val ) );
}

QWidget * ArrayViewItem::getWidget()
{
  return m_widget;
}

void ArrayViewItem::onModelValueChanged( QVariant const &value )
{
  RTVariant::toRTVal( value, m_val );

  // If we have changed our min/max values, then
  // rebuild the whole widget.  Otherwise, update children
  int arraySize = m_val.getArraySize();
  if (arraySize < m_max)
  {
    m_max = arraySize;
    m_min = min( m_max, m_min );
    emit rebuildChildren( this );
  }
  else
  {
    for (int i = m_min; i < m_max; ++i)
    {
      FabricCore::RTVal childVal = m_val.getArrayElementRef( i );
      routeModelValueChanged( i - m_min, toVariant( childVal ) );
    }
  }

  updateWidgets();
}

void ArrayViewItem::updateWidgets()
{
  int arraySize = 0;
  if (m_val.isValid() && m_val.isArray())
    arraySize = m_val.getArraySize();

  m_max = min( m_max, arraySize );
  m_min = min( m_min, m_max );

  m_minIndexEdit->setRange( 0, arraySize );
  m_maxIndexEdit->setRange( 0, arraySize );
  m_arraySizeEdit->setRange( 0, INT_MAX );
  
  m_minIndexEdit->setValue( m_min );
  m_maxIndexEdit->setValue( m_max );
  m_arraySizeEdit->setValue( arraySize );
}

//bool ArrayViewItem::updateAndLimitMinMax()
//{
//  m_min = m_max = 0;
//  if (!m_val.isValid())
//    return false;
//
//  bool ok = true;
//  int min = m_minIndexEdit->text().toInt( &ok );
//    return false;
//
//  int max = m_maxIndexEdit->text().toInt( &ok );
//  if (ok)
//    return false;
//
//  unsigned childCount = m_val.getArraySize();
//  // Limit max/min to appropriate size
//  max = (max > childCount) ? childCount : max;
//  min = (min < max) ? min : max;
//
//  if (min != m_min || max != m_max)
//  {
//    m_min = min;
//    m_max = max;
//    return true;
//  }
//  return false;
//}



void ArrayViewItem::onMinIndexChanged( int newMin )
{
  if (newMin == m_min)
    return;
  m_min = newMin;
  updateWidgets();

  emit rebuildChildren( this );
}

void ArrayViewItem::onMaxIndexChanged( int newMax )
{
  if (newMax == m_max)
    return;

  m_max = newMax;
  updateWidgets();

  emit rebuildChildren( this );
}

void ArrayViewItem::onArraySizeChanged( int newSize )
{
  if (m_val.isValid())
  {
    int oldSize = m_val.getArraySize();

    m_val.setArraySize( newSize );

    emit viewValueChanged( toVariant( m_val ) );

    if (oldSize == m_max)
    {
      onMaxIndexChanged( newSize );
    }
  }
}

//////////////////////////////////////////////////////////
//
BaseViewItem* ArrayViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
{
  if (value.type() != QVariant::UserType)
    return NULL;
  if (value.userType() != qMetaTypeId<FabricCore::RTVal>())
    return NULL;

  FabricCore::RTVal rtVal = value.value<FabricCore::RTVal>();
  if (rtVal.isValid() && rtVal.isArray())
  {
    ArrayViewItem* pViewItem = new ArrayViewItem( QString( name ), rtVal, metadata );
    return pViewItem;
  }
  return NULL;
}

const int ArrayViewItem::Priority = 5;
