//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ModelItems/RefModelItem.h>
#include <FabricUI/ModelItems/RefItemMetadata.h>

namespace FabricUI {
namespace ModelItems {

class SetModelItem : public RefModelItem
{
public:

  SetModelItem(
    DFG::DFGUICmdHandler *dfgUICmdHandler,
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec,
    FTL::StrRef nodeName
    );
  ~SetModelItem();

  virtual BaseModelItem* createChild( FTL::CStrRef name ) /*override*/;
};

} // namespace ModelItems
} // namespace FabricUI
