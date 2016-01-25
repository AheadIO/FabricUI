//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "VEBaseSpinBox.h"

// The Int version of the spinbox just needs to implement the signals
// defined in VEBaseSpinBox
class VEDoubleSpinBox : public VEBaseSpinBox<QDoubleSpinBox, double>
{
  Q_OBJECT

public:

  VEDoubleSpinBox();
  ~VEDoubleSpinBox();

  QString textFromValue( double val ) const;

  virtual double implicitBaseChangePerStep() /*override*/;

  virtual void updateStep(
    double deltaXInInches,
    double sensitivity
    ) /*override*/;

signals:

  void interactionBegin();
  void interactionEnd( bool );

private:

  static double const MAX_QT_VAL;
  static int const MAX_QT_EXP;
};
