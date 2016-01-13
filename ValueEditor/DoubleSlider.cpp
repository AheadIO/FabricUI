//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "DoubleSlider.h"

DoubleSlider::DoubleSlider( QWidget * parent ) 
  : QSlider( parent )
  , m_resolution(100)
  , m_min(0)
  , m_max(1)
{
  // Default to horizontal orientation
  setOrientation( Qt::Horizontal );

  connect( this, SIGNAL( valueChanged( int ) ),
           this, SLOT( notifyValueChanged( int ) ) );
}

void DoubleSlider::mousePressEvent( QMouseEvent *event )
{
  // Taken pretty much verbatim from:
  // http://stackoverflow.com/questions/11132597/qslider-mouse-direct-jump
  QStyleOptionSlider opt;
  initStyleOption( &opt );
  QRect sr = style()->subControlRect( QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this );

  if (event->button() == Qt::LeftButton &&
        sr.contains( event->pos() ) == false)
  {
    int newVal;
    if (orientation() == Qt::Vertical)
      newVal = minimum() + ((maximum() - minimum()) * (height() - event->y())) / height();
    else
      newVal = minimum() + ((maximum() - minimum()) * event->x()) / width();

    if (invertedAppearance() == true)
      setValue( maximum() - newVal );
    else
      setValue( newVal );

    event->accept();
  }
  QSlider::mousePressEvent( event );
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
