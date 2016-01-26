//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "DoubleSlider.h"
#include "FloatSliderViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"
#include "VELineEdit.h"

#include <FabricUI/Util/UIRange.h>
#include <float.h>
#include <FTL/JSONValue.h>
#include <QtCore/QVariant>
#include <QtGui/QHBoxLayout>

FloatSliderViewItem::FloatSliderViewItem(
  QString const &name,
  QVariant const &variant,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
{
  m_lineEdit = new VELineEdit;
  m_slider = new DoubleSlider;

  metadataChanged();

  double value = variant.value<double>();

  m_lineEdit->setText( QString::number( value ) );
  m_slider->setDoubleValue( value );

  m_widget = new QWidget;
  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  layout->addWidget( m_lineEdit );
  layout->addWidget( m_slider );

  connect(
    m_lineEdit, SIGNAL( textModified( QString ) ),
    this, SLOT( onLineEditTextModified( QString ) )
    );

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
}

FloatSliderViewItem::~FloatSliderViewItem()
{
}

QWidget *FloatSliderViewItem::getWidget()
{
  return m_widget;
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

void FloatSliderViewItem::onLineEditTextModified( QString text )
{
  m_slider->setDoubleValue( text.toDouble() );
}

void FloatSliderViewItem::onSliderPressed()
{
  emit interactionBegin();
}

void FloatSliderViewItem::onDoubleValueChanged( double value )
{
  m_lineEdit->setText( QString::number( value ) );
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
