//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "DoubleSlider.h"

#include <math.h>
#include <QtGui/QStyleOptionSlider>
#include <QtGui/QMouseEvent>

#include <FTL/AutoSet.h>

DoubleSlider::DoubleSlider( QWidget * parent )
  : QSlider( parent )
  , m_min( 0.0 )
  , m_max( 1.0 )
  , m_isSettingValue( false )
{
  setResolution( 2, m_min, m_max );
  setFocusPolicy(Qt::StrongFocus);

  // Default to horizontal orientation
  setOrientation( Qt::Horizontal );

  connect( this, SIGNAL( valueChanged( int ) ),
           this, SLOT( onValueChanged( int ) ) );
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

void DoubleSlider::wheelEvent( QWheelEvent *event )
{
  // [FE-5997] inspired by http://stackoverflow.com/questions/5821802/qspinbox-inside-a-qscrollarea-how-to-prevent-spin-box-from-stealing-focus-when
  if (!hasFocus())
  {
    event->ignore();
    return;
  }

  wheelEvent( event );
}

void DoubleSlider::setResolution( int decimals, double min, double max )
{
  double divisor = pow(10.0, (double)decimals);
  double resolution = (max - min) * divisor;
  setRange( 0, (int)floor(resolution+0.5) );
  m_max = max;
  m_min = min;
  setDoubleValue( m_value, false );
}

void DoubleSlider::setDoubleValue( double newDoubleValue, bool emitSignal )
{
  FTL::AutoSet<bool> settingValue(m_isSettingValue, true);

  m_value = newDoubleValue;
  newDoubleValue = std::max( m_min, std::min( m_max, newDoubleValue ) );
  double ratio = (newDoubleValue - m_min) / (m_max - m_min);
  int newIntValue = int( round( ratio * maximum() ) );
  if ( value() != newIntValue ) {
    setValue( newIntValue );
  }
  if(emitSignal)
    emit doubleValueChanged( m_value );
}

double DoubleSlider::doubleValue()
{
  return m_value;
}

double DoubleSlider::toDouble( int value )
{
  double ratio = double( value ) / double( maximum() );
  return (1.0 - ratio) * m_min + ratio * m_max;
}

void DoubleSlider::onValueChanged( int value ) {
  if(m_isSettingValue)
    return;
  m_value = toDouble( value );
  emit doubleValueChanged( m_value );
}
