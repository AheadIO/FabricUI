//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include "NodePortItemMetadata.h"

#include <FabricUI/ModelItems/GetPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

class GetPortItemMetadata : public NodePortItemMetadata
{
public:

  GetPortItemMetadata( GetPortModelItem *getPortModelItem )
    : NodePortItemMetadata( getPortModelItem ) {}

  virtual const char* getString( const char* key ) const /*override*/
  {
    if ( key == VENotInspectableKey )
    {
      return FTL_STR("1").c_str();
    }

    return NodePortItemMetadata::getString( key );
  }
};

} // namespace ModelItems
} // namespace FabricUI
