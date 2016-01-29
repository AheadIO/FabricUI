//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "VEIntSpinBox.h"

#include <limits.h>

//////////////////////////////////////////////////////////////////////////
VEIntSpinBox::VEIntSpinBox( )
{
  setRange( INT_MIN, INT_MAX );
}

VEIntSpinBox::~VEIntSpinBox()
{
}

double VEIntSpinBox::implicitLogBaseChangePerStep()
{
  if ( minimum() == INT_MIN
    || maximum() == INT_MAX )
    return 0.0;

  return std::max(
    0.0,
    log10( ( maximum() - minimum() ) / 100.0 )
    );
}

double VEIntSpinBox::updateStep(
  double deltaXInInches,
  double logBaseChangePerStep
  )
{
  double base10Exp = 0.5 * deltaXInInches + logBaseChangePerStep;
  double velocity = pow( 10, base10Exp );

  // Always step by a round-number 
  int roundedBase10Exp = int( round( base10Exp ) );
  int changePerStep = int( trunc( pow( 10.0, roundedBase10Exp ) ) );

  static const int minChangePerStep = 1;
  changePerStep = std::max( minChangePerStep, changePerStep );
  static const int maxChangePerStep = 1000;
  changePerStep = std::min( maxChangePerStep, changePerStep );

  setSingleStep( changePerStep );

  return std::max(
    0.01,
    0.5 * velocity / changePerStep
    );
}
