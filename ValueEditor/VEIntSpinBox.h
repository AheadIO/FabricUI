//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "VEBaseSpinBox.h"

// The Int version of the spinbox just needs to implement the signals
// defined in VEBaseSpinBox
class VEIntSpinBox : public VEBaseSpinBox<QSpinBox, int>
{
  Q_OBJECT

public:

  VEIntSpinBox();
  ~VEIntSpinBox();

  void updateStep( double deltaXInInches, double sensitivity );

signals:

  void interactionBegin();
  void interactionEnd( bool );
};
