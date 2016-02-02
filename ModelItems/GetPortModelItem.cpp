//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/GetPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
GetPortModelItem::GetPortModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName,
  FTL::StrRef portName
  )
  : RefPortModelItem(
    dfgUICmdHandler,
    binding,
    execPath,
    exec,
    nodeName,
    portName
    )
{
}

GetPortModelItem::~GetPortModelItem()
{
}

FTL::CStrRef GetPortModelItem::getName()
{
  return FTL_STR("value");
}

} // namespace ModelItems
} // namespace FabricUI
