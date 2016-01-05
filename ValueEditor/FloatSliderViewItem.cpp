#include "stdafx.h"
#include "FloatSliderViewItem.h"
#include "BaseViewItemCreator.h"
#include "DoubleSlider.h"

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

void FloatSliderViewItem::onModelValueChanged( QVariant const &value )
{
  m_slider->setDoubleValue( value.toDouble() );
}

void FloatSliderViewItem::updateMetadata( FTL::JSONObject* metaData ) 
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
static FloatSliderViewItem* CreateItem(
  QString const &name,
  QVariant const &value,
  FTL::JSONObject* metaData
  )
{
  if (metaData == NULL)
    return NULL;

  if (value.type() == QVariant::Double
       || value.type() == QMetaType::Float)
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

EXPOSE_VIEW_ITEM( FloatSliderViewItem, CreateItem, 5 );

// Include MOC'ed file here, in order
// to support PCH on windows.
#include "moc_FloatSliderViewItem.cpp"