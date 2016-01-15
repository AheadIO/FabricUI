//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "DoubleSlider.h"
#include "FloatSliderViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"

#include <FTL/JSONValue.h>
#include <QtCore/QVariant>

FloatSliderViewItem::FloatSliderViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem( name )
{
  m_slider = new DoubleSlider;
  connect(
    m_slider, SIGNAL( doubleValueChanged( double ) ),
    this, SLOT( OnSpinnerChanged( double ) )
    );
  connect(
    m_slider, SIGNAL( sliderReleased() ),
    this, SLOT( OnEditFinished() )
    );
  onModelValueChanged( value );
}

FloatSliderViewItem::~FloatSliderViewItem()
{
}

QWidget *FloatSliderViewItem::getWidget()
{
  return m_slider;
}

void FloatSliderViewItem::onModelValueChanged( QVariant const &v )
{
  m_slider->setDoubleValue( v.value<double>() );
}

void FloatSliderViewItem::updateMetadata( ItemMetadata* metaData ) 
{
  if (metaData->has( "min" ) && metaData->has( "max" ))
  {
    double min = metaData->getFloat64( "min" );
    double max = metaData->getFloat64( "max" );
    m_slider->setResolution( 100, min, max );
  }
}

void FloatSliderViewItem::OnSpinnerChanged( double value )
{
  emit viewValueChanged(
    QVariant::fromValue<double>( value ),
    0
    );
}

void FloatSliderViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue<double>( m_slider->doubleValue() ),
    1
    );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* FloatSliderViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if (metaData == NULL)
    return NULL;

  if (RTVariant::isType<double>(value) || RTVariant::isType<float>(value))
  {
    // We can only create the UI if we have a defined Min & Max
    if (metaData->has( "min" ) && metaData->has( "max" ))
    {
      FloatSliderViewItem* item = new FloatSliderViewItem( name, value );
      item->updateMetadata( metaData );
      return item;
    }
  }
  return NULL;
}

const int FloatSliderViewItem::Priority = 5;
