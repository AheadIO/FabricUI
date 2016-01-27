//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
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

  virtual double implicitLogBaseChangePerStep() /*override*/;

  virtual double updateStep(
    double deltaXInInches,
    double logBaseChangePerStep
    ) /*override*/;

  virtual void resetStep() /*override*/;

signals:

  void interactionBegin();
  void interactionEnd( bool );

protected:

  virtual void emitInteractionBegin() /*override*/
    { emit interactionBegin(); }

  virtual void emitInteractionEnd( bool commit ) /*override*/
    { emit interactionEnd( commit ); }

private:

  static double const MAX_QT_VAL;
  static int const MAX_QT_EXP;
};
