//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "IntSliderViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"

#include <QtCore/QVariant>
#include <QtGui/QSlider>

IntSliderViewItem::IntSliderViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
{
  m_slider = new QSlider;

  // Default to horizontal orientation
  m_slider->setOrientation( Qt::Horizontal );

  connect(
    m_slider, SIGNAL( valueChanged( int ) ),
    this, SLOT( OnSpinnerChanged( int ) )
    );
  connect(
    m_slider, SIGNAL( sliderReleased() ),
    this, SLOT( OnEditFinished() )
    );
  metadataChanged();
  onModelValueChanged( value );
}

IntSliderViewItem::~IntSliderViewItem()
{
}

QWidget *IntSliderViewItem::getWidget()
{
  return m_slider;
}

void IntSliderViewItem::onModelValueChanged( QVariant const &value )
{
  m_slider->setValue( value.toInt() );
}

void IntSliderViewItem::metadataChanged()
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
      int min = minStr.toInt( &ok );
      if (ok)
      {
        int max = maxStr.toInt( &ok );
        if (ok)
          m_slider->setRange( min, max );
      }
    }
    catch (...)
    {
      // No change on exception
      printf( "[ERROR] Unknown error setting range on slider" );
    }
  }
}

void IntSliderViewItem::OnSpinnerChanged( int value )
{
  emit viewValueChanged(
    QVariant::fromValue( value )
    );
}

void IntSliderViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue( m_slider->value() )
    );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* IntSliderViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if (metaData == NULL)
    return NULL;

  if ( RTVariant::isType<int>( value )
    || RTVariant::isType<unsigned>( value ) )
  {
    if (metaData->has( "uiRange" ))
    {
      IntSliderViewItem* item = new IntSliderViewItem( name, value, metaData );
      return item;
    }
  }
  return NULL;
}

const int IntSliderViewItem::Priority = 5;
