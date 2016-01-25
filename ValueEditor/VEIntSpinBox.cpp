//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
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

void VEIntSpinBox::updateStep()
{
  // Always step by a round-number 
  int exp = log10( abs( m_startValue ) ) - 2;
  int changePerStep = pow( 10.0, exp );
  static const int minChangePerStep = 1;
  static const int maxChangePerStep = 1000;
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
