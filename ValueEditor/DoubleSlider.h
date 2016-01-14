//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtGui/QSlider.h>

class DoubleSlider : public QSlider {
  Q_OBJECT

  int m_resolution;
  double m_min;
  double m_max;

public:
  DoubleSlider( QWidget *parent = 0 );

  // Override the slider positioning to make the slider 
  // go directly to the clicked positioning (instead of
  // going there by incremenets)
  virtual void mousePressEvent( QMouseEvent * ) /*override*/;

  void setResolution( int resolution, double min, double max );

  void setDoubleValue( double value );
  double doubleValue();
  double toDouble( int value );


signals :
  void doubleValueChanged( double value);

public slots:
  void notifyValueChanged( int value );


};
