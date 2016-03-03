//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtGui/QSlider>
#include <QtGui/qevent.h>

class IntSlider : public QSlider {
  Q_OBJECT

public:

  IntSlider( QWidget *parent = 0 );

  virtual void wheelEvent( QWheelEvent *event ) /*override*/;
};
