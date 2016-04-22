//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_UTIL_UIRANGE_H
#define FABRICUI_UTIL_UIRANGE_H

#include <FTL/StrRef.h>
#include <math.h>

namespace FabricUI {

//
// Parse a uiRange metadata attribute into minValue and maxValue components
// Returns true on success and false on failure
//
inline bool DecodeUIRange(
  FTL::StrRef str,
  double &minValue,
  double &maxValue
  )
{
  str = str.trim();

  if ( str.empty() || str.front() != '(' )
    return false;
  str = str.drop_front().trim();

  if ( str.empty() || str.back() != ')' )
    return false;
  str = str.drop_back().trim();

  FTL::StrRef::Split split = str.trimSplit( ',' );

  if ( split.first.empty() )
    return false;
  minValue = split.first.toFloat64();

  if ( split.second.empty() )
    return false;
  maxValue = split.second.toFloat64();

  return minValue <= maxValue;
}

inline bool DecodeUIRange(
  FTL::StrRef str,
  int &minValue,
  int &maxValue
  )
{
  double doubleMinValue, doubleMaxValue;
  bool result = DecodeUIRange( str, doubleMinValue, doubleMaxValue );
  if ( result )
  {
    minValue = int( floor( doubleMinValue ) );
    maxValue = int( ceil( doubleMaxValue ) );
  }
  return result;
}

}

#endif // FABRICUI_UTIL_UIRANGE_H
