//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "IntSliderViewItem.h"
#include "ViewConstants.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"
#include "VELineEdit.h"

#include <limits.h>
#include <QtCore/QVariant>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSlider>

IntSliderViewItem::IntSliderViewItem(
  QString const &name,
  QVariant const &variant,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
{
  m_lineEdit = new VELineEdit;
  m_slider = new QSlider;
  m_slider->setOrientation( Qt::Horizontal );

  metadataChanged();

  int value = variant.value<int>();

  m_lineEdit->setText( QString::number( value ) );
  m_slider->setValue( value );

  m_widget = new QWidget;
  QHBoxLayout *layout = new QHBoxLayout( m_widget );

  QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);

  sp.setHorizontalStretch(1);
  m_lineEdit->setSizePolicy(sp);
  layout->addWidget( m_lineEdit );

  sp.setHorizontalStretch(3);
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
  m_slider->setValue( v.value<int>() );
}

void IntSliderViewItem::onLineEditTextModified( QString text )
{
  int value = std::max(
    m_slider->minimum(),
    std::min(
      m_slider->maximum(),
      text.toInt()
      )
    );
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
  m_lineEdit->setText( QString::number( value ) );
  emit viewValueChanged( QVariant::fromValue<int>( value ) );
}

void IntSliderViewItem::onSliderReleased()
{
  emit interactionEnd( true );
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
