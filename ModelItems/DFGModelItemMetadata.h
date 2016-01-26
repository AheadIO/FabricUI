//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <assert.h>
#include <FabricCore.h>
#include <FabricUI/ModelItems/ModelItemMetadata.h>
#include <FTL/StrRef.h>

namespace FabricUI {
namespace ModelItems {

class DFGModelItemMetadata : public ModelItemMetadata
{
protected:

  static FTL::CStrRef DFGPortTypeToVEPortType(
    FabricCore::DFGPortType dfgPortType
    )
  {
    switch ( dfgPortType )
    {
      case FabricCore::DFGPortType_In:
        return FTL_STR("In").c_str();
      case FabricCore::DFGPortType_IO:
        return FTL_STR("IO").c_str();
      case FabricCore::DFGPortType_Out:
        return FTL_STR("Out").c_str();
      default:
        assert( false );
        return FTL_STR("").c_str();
    }
  }

public:

  DFGModelItemMetadata() {}
};

} // namespace ModelItems
} // namespace FabricUI
