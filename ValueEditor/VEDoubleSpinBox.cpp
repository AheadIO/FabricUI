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

void VEDoubleSpinBox::updateStep( double deltaXInInches, double sensitivity )
{
  // Always step by a round-number 
  int base10Exp = int( trunc( 0.5 * deltaXInInches ) );
  double changePerStep = sensitivity * pow( 10.0, base10Exp );
  static const double minChangePerStep = 0.00001;
  changePerStep = std::max( minChangePerStep, changePerStep );
  static const double maxChangePerStep = 100000;
  changePerStep = std::min( maxChangePerStep, changePerStep );
  setSingleStep( changePerStep );
}
