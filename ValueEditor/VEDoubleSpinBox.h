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

  virtual void updateStep();

signals:

  void interactionBegin();
  void interactionEnd( bool );
};
