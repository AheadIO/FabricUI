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
  const char* str = m_metadata.getString( "uiRange" );
  if (str == NULL)
    return;

  std::string rangeStr = str;
  if (rangeStr.size() > 0)
  {
    try 
    {
      if (rangeStr[0] == '(')
        rangeStr = rangeStr.substr( 1 );
      if (rangeStr[rangeStr.size() - 1] == ')')
        rangeStr = rangeStr.substr( 0, rangeStr.size() - 1 );

      QStringList parts = QString( rangeStr.c_str() ).split( ',' );
      if (parts.size() != 2)
        return;

      QString minStr = parts[0].trimmed();
      QString maxStr = parts[1].trimmed();
      bool ok;
      double min = minStr.toFloat(&ok);
      if (ok)
      {
        double max = maxStr.toFloat(&ok);
        if (ok)
          m_slider->setResolution( 100, min, max );
      }
    }
    catch (...)
    {
      // No change on exception
      printf( "[ERROR] Unknown error setting range on slider" );
    }
  }
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
