#include "stdafx.h"
#include "DoubleSlider.h"

DoubleSlider::DoubleSlider( QWidget * parent ) 
  : QSlider( parent )
  , m_resolution(100)
  , m_min(0)
  , m_max(1)
{
  connect( this, SIGNAL( valueChanged( int ) ),
           this, SLOT( notifyValueChanged( int ) ) );
}

void DoubleSlider::setResolution( int resolution, double min, double max )
{
  m_resolution = resolution;
  m_max = max;
  m_min = min;
}

void DoubleSlider::setDoubleValue( double value )
{
  if (value < m_min)
    setValue( 0 );
  else if (value > m_max)
    setValue( m_resolution );
  else
  {
    double ratio = (value - m_min) / (m_max - m_min);
    setValue( int(ratio * m_resolution) );
  }

  // Default to horizontal orientation
  setOrientation( Qt::Horizontal );
}

double DoubleSlider::doubleValue()
{
  return toDouble( value() );
}

double DoubleSlider::toDouble( int value )
{
  return m_min + ((m_max - m_min) * value / m_resolution);
}

void DoubleSlider::notifyValueChanged( int value ) {
  emit doubleValueChanged( toDouble( value ) );
}

// Include MOC'ed file here, in order
// to support PCH on windows.
#include "moc_DoubleSlider.cpp"