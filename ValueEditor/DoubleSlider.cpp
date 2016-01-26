//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "DoubleSlider.h"

#include <math.h>
#include <QtGui/QStyleOptionSlider>
#include <QtGui/QMouseEvent>

DoubleSlider::DoubleSlider( QWidget * parent ) 
  : QSlider( parent )
  , m_min( 0.0 )
  , m_max( 1.0 )
{
  setResolution( 100, m_min, m_max );

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
    int max = maximum();

    int newVal = 0;
    if ( orientation() == Qt::Vertical )
    {
      int h = height();
      if ( h > 1 )
      {
        --h;
        newVal += (max * (h - event->y()) + h/2) / h;
      }
    }
    else
    {
      int w = width();
      if ( w > 1 )
      {
        --w;
        newVal += (max * event->x() + w/2) / w;
      }
    }

    if ( invertedAppearance() )
      setValue( max - newVal );
    else
      setValue( newVal );

    event->accept();
  }
  QSlider::mousePressEvent( event );
}


void DoubleSlider::setResolution( int resolution, double min, double max )
{
  double value = doubleValue();
  setRange( 0, resolution );
  m_max = max;
  m_min = min;
  setDoubleValue( value );
}

void DoubleSlider::setDoubleValue( double newDoubleValue )
{
  newDoubleValue = std::max( m_min, std::min( m_max, newDoubleValue ) );
  double ratio = (newDoubleValue - m_min) / (m_max - m_min);
  int newIntValue = int( round( ratio * maximum() ) );
  if ( value() != newIntValue )
    setValue( newIntValue );
  else
    emit valueChanged( newIntValue );
}

double DoubleSlider::doubleValue()
{
  return toDouble( value() );
}

double DoubleSlider::toDouble( int value )
{
  double ratio = double( value ) / double( maximum() );
  return (1.0 - ratio) * m_min + ratio * m_max;
}

void DoubleSlider::notifyValueChanged( int value ) {
  emit doubleValueChanged( toDouble( value ) );
}
