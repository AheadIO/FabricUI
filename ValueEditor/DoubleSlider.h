//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtGui/QSlider>
#include <QtGui/qevent.h>


namespace FabricUI {
namespace ValueEditor {

class DoubleSlider : public QSlider {
  Q_OBJECT

  double m_min;
  double m_max;

public:

  DoubleSlider( QWidget *parent = 0 );

  // Override the slider positioning to make the slider 
  // go directly to the clicked positioning (instead of
  // going there by incremenets)
  virtual void mousePressEvent( QMouseEvent * ) /*override*/;

  virtual void wheelEvent( QWheelEvent *event ) /*override*/;

  void setResolution( int decimals, double min, double max );

  void setDoubleValue( double value, bool emitSignal = true );
  double doubleValue();
  double toDouble( int value );

  double min() const
    { return m_min; }
  double max() const
    { return m_max; }

signals :
  void doubleValueChanged( double value);

public slots:
  void onValueChanged( int value );

private:

  double m_value;
  bool m_isSettingValue;
};

} // namespace FabricUI 
} // namespace ValueEditor 
