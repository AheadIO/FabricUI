//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include "NodePortItemMetadata.h"

#include <FabricUI/ModelItems/RefPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

class RefPortItemMetadata : public NodePortItemMetadata
{
public:

  RefPortItemMetadata( RefPortModelItem *refPortModelItem )
    : NodePortItemMetadata( refPortModelItem ) {}

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
