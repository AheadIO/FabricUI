//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include "NodePortItemMetadata.h"

#include <FabricUI/ModelItems/VarPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

class VarPortItemMetadata : public NodePortItemMetadata
{
public:

  VarPortItemMetadata( VarPortModelItem *varPortModelItem )
    : NodePortItemMetadata( varPortModelItem ) {}

  virtual const char* getString( const char* key ) const /*override*/
  {
    if ( key == VENotInspectableKey )
    {
      return FTL_STR("").c_str();
    }

    return NodePortItemMetadata::getString( key );
  }
};

} // namespace ModelItems
} // namespace FabricUI
