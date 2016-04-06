//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include "VEBaseSpinBox.h"

namespace FabricUI {
namespace ValueEditor {

// The Int version of the spinbox just needs to implement the signals
// defined in VEBaseSpinBox
class VEIntSpinBox : public VEBaseSpinBox<QSpinBox, int>
{
  Q_OBJECT

public:

  VEIntSpinBox();
  ~VEIntSpinBox();

  virtual double implicitLogBaseChangePerStep() /*override*/;

  virtual double updateStep(
    double deltaXInInches,
    double logBaseChangePerStep
    ) /*override*/;

signals:

  void interactionBegin();
  void interactionEnd( bool commit );

//////////////////////////////////////////////////////////////////////////////
// All this stuff should be in the base class or a macro, but both mess up Qt
//////////////////////////////////////////////////////////////////////////////

protected:
  
  virtual void connectSteppingTimer( QTimer *steppingTimer ) /*override*/
  {
    connect(
      steppingTimer, SIGNAL(timeout()),
      this, SLOT(onSteppingTimeout())
      );
  }
  virtual void emitInteractionBegin() /*override*/
    { emit interactionBegin(); }
  virtual void emitInteractionEnd( bool commit ) /*override*/
    { emit interactionEnd( commit ); }

protected slots:
  
  void onSteppingTimeout()
    { endStepping(); }

//////////////////////////////////////////////////////////////////////////////
};

} // namespace FabricUI 
} // namespace ValueEditor 
