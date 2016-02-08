//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "IntSliderViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"
#include "VELineEdit.h"
#include "ViewConstants.h"

#include <limits.h>
#include <QtCore/QVariant>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSlider>
#include <QtGui/QIntValidator>

#include <FabricUI/Util/UIRange.h>
#include <FTL/AutoSet.h>

IntSliderViewItem::IntSliderViewItem(
  QString const &name,
  QVariant const &variant,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
  , m_isSettingValue( false )
{
  m_lineEdit = new VELineEdit;
  m_slider = new QSlider;
  m_slider->setOrientation( Qt::Horizontal );

  metadataChanged();

  int value = variant.value<int>();

  // correct the softrange
  if(value < m_slider->minimum())
    m_slider->setMinimum(value);
  if(value > m_slider->maximum())
    m_slider->setMaximum(value);

  m_lineEdit->setValidator(new QIntValidator(m_lineEdit));
  m_lineEdit->setText( QString::number( value ) );
  m_slider->setValue( value );

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
    m_slider, SIGNAL( valueChanged( int ) ),
    this, SLOT( onValueChanged( int ) )
    );
  connect(
    m_slider, SIGNAL( sliderReleased() ),
    this, SLOT( onSliderReleased() )
    );
}

IntSliderViewItem::~IntSliderViewItem()
{
}

QWidget *IntSliderViewItem::getWidget()
{
  return m_widget;
}

void IntSliderViewItem::onModelValueChanged( QVariant const &v )
{
  FTL::AutoSet<bool> settingValue(m_isSettingValue, true);
  int value = v.value<int>();

  // correct the softrange
  if(value < m_slider->minimum())
    m_slider->setMinimum(value);
  if(value > m_slider->maximum())
    m_slider->setMaximum(value);

  m_slider->setValue( value );
  m_lineEdit->setText( QString::number( value ) );
}

void IntSliderViewItem::onLineEditTextModified( QString text )
{
  int value = std::max(
    m_hardMinimum,
    std::min(
      m_hardMaximum,
      text.toInt()
      )
    );

  // correct the softrange
  if(value < m_slider->minimum())
    m_slider->setMinimum(value);
  if(value > m_slider->maximum())
    m_slider->setMaximum(value);

  if ( value != m_slider->value() )
    m_slider->setValue( value );
  else
    m_lineEdit->setText( QString::number( value ) );
}

void IntSliderViewItem::onSliderPressed()
{
  emit interactionBegin();
}

void IntSliderViewItem::onValueChanged( int value )
{
  if(m_isSettingValue)
    return;
  m_lineEdit->setText( QString::number( value ) );
  emit viewValueChanged( QVariant::fromValue<int>( value ) );
}

void IntSliderViewItem::onSliderReleased()
{
  emit interactionEnd( true );
}

void IntSliderViewItem::metadataChanged()
{
  m_slider->setRange( m_softMinimum, m_softMaximum );

  // metadata should be like this:
  // uiRange: "(0, 200)"
  // uiHardRange: "(0, 500)"

  m_hardMinimum = INT_MIN;
  m_hardMaximum = INT_MAX;
  FTL::CStrRef hardRangeCStr = m_metadata.getString( "uiHardRange" );
  bool decodedHard =
    FabricUI::DecodeUIRange( hardRangeCStr, m_hardMinimum, m_hardMaximum );

  m_softMinimum = 0;
  m_softMaximum = 100;
  FTL::CStrRef softRangeCStr = m_metadata.getString( "uiRange" );
  bool decodedSoft =
    FabricUI::DecodeUIRange( softRangeCStr, m_softMinimum, m_softMaximum );

  if ( decodedHard && !decodedSoft )
  {
    m_softMinimum = m_hardMinimum;
    m_softMaximum = m_hardMaximum;
  }
  else
  {
    m_softMinimum = std::max( m_softMinimum, m_hardMinimum );
    m_softMaximum = std::min( m_softMaximum, m_hardMaximum );
  }

  m_slider->setRange( m_softMinimum, m_softMaximum );
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
    if (metaData->has( "uiRange" ) || metaData->has( "uiHardRange" ))
    {
      IntSliderViewItem* item = new IntSliderViewItem( name, value, metaData );
      return item;
    }
  }
  return NULL;
}

const int IntSliderViewItem::Priority = 5;
