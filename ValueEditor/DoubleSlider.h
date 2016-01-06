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

  void setResolution( int resolution, double min, double max );

  void setDoubleValue( double value );
  double doubleValue();
  double toDouble( int value );

signals :
  void doubleValueChanged( double value);

public slots:
  void notifyValueChanged( int value );
};
