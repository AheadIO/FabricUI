//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "DoubleSlider.h"
#include "ViewConstants.h"
#include "FloatSliderViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"
#include "VELineEdit.h"

#include <FabricUI/Util/UIRange.h>
#include <float.h>
#include <FTL/JSONValue.h>
#include <FTL/AutoSet.h>
#include <QtCore/QVariant>
#include <QtGui/QHBoxLayout>

FloatSliderViewItem::FloatSliderViewItem(
  QString const &name,
  QVariant const &variant,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
  , m_isSettingValue(false)
{
  m_lineEdit = new VELineEdit;
  m_slider = new DoubleSlider;

  metadataChanged();

  double value = variant.value<double>();

  // correct the softrange
  if(value < m_slider->min())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, value, m_slider->max());
  if(value > m_slider->max())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, m_slider->min(), value);

  m_lineEdit->setText( QString::number( value ) );
  m_slider->setDoubleValue( value );

  m_widget = new QWidget;
  QHBoxLayout *layout = new QHBoxLayout( m_widget );

  QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);

  sp.setHorizontalStretch(VE_H_STRETCH_SLIDER_LINEEDIT);
  m_lineEdit->setSizePolicy(sp);
  layout->addWidget( m_lineEdit );

  sp.setHorizontalStretch(VE_H_STRETCH_SLIDER_SLIDER);
  m_slider->setSizePolicy(sp);
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
  FTL::AutoSet<bool> settingValue(m_isSettingValue, true);

  double value = v.value<double>();

  // correct the softrange
  if(value < m_slider->min())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, value, m_slider->max());
  if(value > m_slider->max())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, m_slider->min(), value);

  m_slider->setDoubleValue( value, false );  
  m_lineEdit->setText( QString::number( value ) );
}

void FloatSliderViewItem::metadataChanged()
{
  m_softMinimum = (double)0.0;
  m_softMaximum = (double)1.0;
  m_hardMinimum = DBL_MIN;
  m_hardMaximum = DBL_MAX;

  m_slider->setResolution( FLOAT_SLIDER_DECIMALS, m_softMinimum, m_softMaximum );

  // metadata should be like this:
  // uiRange: "(0 - 200)"
  // uiHardRange: "(0 - 500)"
  const char* softRangeCStr = m_metadata.getString( "uiRange" );
  const char* hardRangeCStr = m_metadata.getString( "uiHardRange" );
  if(FTL::StrRef(softRangeCStr).empty())
    softRangeCStr = hardRangeCStr;

  FabricUI::DecodeUIRange(hardRangeCStr, m_hardMinimum, m_hardMaximum);
  if(FabricUI::DecodeUIRange(softRangeCStr, m_softMinimum, m_softMaximum))
    m_slider->setResolution( FLOAT_SLIDER_DECIMALS, m_softMinimum, m_softMaximum );
}

void FloatSliderViewItem::onLineEditTextModified( QString text )
{
  double value = std::max(
    m_hardMinimum,
    std::min(
      m_hardMaximum,
      text.toDouble()
      )
    );

  // correct the softrange
  if(value < m_slider->min())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, value, m_slider->max());
  if(value > m_slider->max())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, m_slider->min(), value);

  if ( value != m_slider->doubleValue() )
    m_slider->setDoubleValue( value, true /* emitSignal */ );
  else
    m_lineEdit->setText( QString::number( value ) );
}

void FloatSliderViewItem::onSliderPressed()
{
  emit interactionBegin();
}

void FloatSliderViewItem::onDoubleValueChanged( double value )
{
  if(m_isSettingValue)
    return;
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
    if (metaData->has( "uiRange" ) || metaData->has( "uiHardRange" ))
    {
      FloatSliderViewItem* item = new FloatSliderViewItem( name, value, metaData );
      return item;
    }
  }
  return NULL;
}

const int FloatSliderViewItem::Priority = 5;
