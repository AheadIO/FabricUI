#include "VEDoubleSpinBox.h"
#include "VEBaseSpinBox.hpp"
#include <float.h>

//////////////////////////////////////////////////////////////////////////
VEDoubleSpinBox::VEDoubleSpinBox()
{
  // Reset defaults to allow very big input
  setRange( DBL_MIN, DBL_MAX );
  setDecimals( 10 );
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
  static const double minChangePerStep = 0.1;
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

