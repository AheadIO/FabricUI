#include "stdafx.h"
#include "IntSliderViewItem.h"
#include "BaseViewItemCreator.h"
#include "ItemMetadata.h"

IntSliderViewItem::IntSliderViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem( name )
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

void IntSliderViewItem::updateMetadata( ItemMetadata* metaData ) 
{
  if (metaData->has( "min" ) && metaData->has( "max" ))
  {
    int min = metaData->getSInt32( "min" );
    int max = metaData->getSInt32( "max" );
    m_slider->setRange( min, max );
  }
}

void IntSliderViewItem::OnSpinnerChanged( int value )
{
  emit viewValueChanged(
    QVariant::fromValue( value ),
    0
    );
}

void IntSliderViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue( m_slider->value() ),
    1
    );
}

//////////////////////////////////////////////////////////////////////////
// 
static IntSliderViewItem* CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if (metaData == NULL)
    return NULL;

  if (value.type() == QVariant::Int
       || value.type() == QMetaType::UInt)
  {
    // We can only create the UI if we have a defined Min & Max
    if (metaData->has( "min" ) && metaData->has( "max" ))
    {
      IntSliderViewItem* item = new IntSliderViewItem( name, value );
      item->updateMetadata( metaData );
      return item;
    }
  }
  return NULL;
}

EXPOSE_VIEW_ITEM( IntSliderViewItem, CreateItem, 5 );

// Include MOC'ed file here, in order
// to support PCH on windows.
#include "moc_IntSliderViewItem.cpp"