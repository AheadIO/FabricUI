//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "VEDoubleSpinBox.h"

#include <float.h>
#include <math.h>

double const VEDoubleSpinBox::MAX_QT_VAL = 999999999.0;
int const VEDoubleSpinBox::MAX_QT_EXP = 10;

VEDoubleSpinBox::VEDoubleSpinBox()
{
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

double VEDoubleSpinBox::implicitBaseChangePerStep()
{
  if ( minimum() == -MAX_QT_VAL
    || maximum() == +MAX_QT_VAL )
    return 1.0;

  return (maximum() - minimum()) / 20.0;
}

void VEDoubleSpinBox::updateStep( double deltaXInInches, double baseChangePerStep )
{
  // Always step by a round-number 
  int base10Exp = int( round( 0.5 * deltaXInInches ) );
  double changePerStep = baseChangePerStep * pow( 10.0, base10Exp );
  static const double minChangePerStep = 0.00001;
  changePerStep = std::max( minChangePerStep, changePerStep );
  static const double maxChangePerStep = 100000;
  changePerStep = std::min( maxChangePerStep, changePerStep );
  setSingleStep( changePerStep );
}
