//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "DoubleSlider.h"
#include "FloatSliderViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"

#include <FabricUI/Util/UIRange.h>
#include <FTL/JSONValue.h>
#include <QtCore/QVariant>

FloatSliderViewItem::FloatSliderViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
{
  m_slider = new DoubleSlider;
  onModelValueChanged( value );

  connect(
    m_slider, SIGNAL( sliderPressed() ),
    this, SLOT( onSliderPressed() )
    );
  connect(
    m_slider, SIGNAL( doubleValueChanged( double ) ),
    this, SLOT( onDoubleValueChanged( double ) )
    );
  connect(
    m_slider, SIGNAL( sliderReleased() ),
    this, SLOT( onSliderReleased() )
    );
  metadataChanged();
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

void FloatSliderViewItem::metadataChanged()
{
  FTL::StrRef uiRangeString = m_metadata.getString( "uiRange" );
  
  double minValue, maxValue;
  if ( FabricUI::DecodeUIRange( uiRangeString, minValue, maxValue ) )
    m_slider->setResolution( 100, minValue, maxValue );
}

void FloatSliderViewItem::onSliderPressed()
{
  emit interactionBegin();
}

void FloatSliderViewItem::onDoubleValueChanged( double value )
{
  emit viewValueChanged( QVariant::fromValue<double>( value ) );
}

void FloatSliderViewItem::onSliderReleased()
{
  emit interactionEnd( true );
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
    if (metaData->has( "uiRange" ) )
    {
      FloatSliderViewItem* item = new FloatSliderViewItem( name, value, metaData );
      return item;
    }
  }
  return NULL;
}

const int FloatSliderViewItem::Priority = 5;
