//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_VEDOUBLESPINBOX_H
#define FABRICUI_VALUEEDITOR_VEDOUBLESPINBOX_H

#include "VEBaseSpinBox.h"

namespace FabricUI {
namespace ValueEditor {

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

  virtual void resetPrecision() /*override*/;

signals:

  void interactionBegin();
  void interactionEnd( bool );

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

private:

  static double const MAX_QT_VAL;
  static int const MAX_QT_EXP;
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_VEDOUBLESPINBOX_H
