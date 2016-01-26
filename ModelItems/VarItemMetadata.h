//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "DFGModelItemMetadata.h"

namespace FabricUI {
namespace ModelItems {

class VarModelItem;

class VarItemMetadata : public DFGModelItemMetadata
{
public:

  VarItemMetadata( VarModelItem *varModelItem ) {}

  virtual const char* getString( const char* key ) const /*override*/
  {
    if ( key == FTL_STR("vePortType") )
      return FTL_STR("IO").c_str();

    return 0;
  }
};

} // namespace ModelItems
} // namespace FabricUI
