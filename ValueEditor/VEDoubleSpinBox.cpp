//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "VEDoubleSpinBox.h"

#include <float.h>
#include <math.h>

VEDoubleSpinBox::VEDoubleSpinBox()
{
  const double MAX_QT_VAL = 999999999;
  const int MAX_QT_EXP = 10;
  setRange( -MAX_QT_VAL, MAX_QT_VAL );
  setDecimals( MAX_QT_EXP );
}

VEDoubleSpinBox::~VEDoubleSpinBox()
{
}

QString VEDoubleSpinBox::textFromValue( double val ) const
{
  return QString::number( val );
}

void VEDoubleSpinBox::updateStep()
{
  // Always step by a round-number 
  int exp = log10f( fabs( m_startValue ) ) - 2;
  double changePerStep = pow( 10.0, exp );
  static const double minChangePerStep = 0.01;
  static const double maxChangePerStep = 1000;
  changePerStep =
    std::min(
      maxChangePerStep,
      std::max(
        minChangePerStep,
        changePerStep
        )
      );
  setSingleStep( changePerStep );
}
