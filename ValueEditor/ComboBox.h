//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtGui/QCombobox>
#include <QtGui/qevent.h>

class ComboBox : public QComboBox {
  Q_OBJECT

public:

  ComboBox( QWidget *parent = 0 );

  virtual void wheelEvent( QWheelEvent *event ) /*override*/;
};
