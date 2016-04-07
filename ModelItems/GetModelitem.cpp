//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/GetModelItem.h>
#include <FabricUI/ModelItems/GetPortModelItem.h>
#include <FabricUI/ModelItems/VarPathModelItem.h>

namespace FabricUI {
namespace ModelItems {

GetModelItem::GetModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName
  )
  : RefModelItem(
    dfgUICmdHandler,
    binding,
    execPath,
    exec,
    nodeName
    )
{
}

GetModelItem::~GetModelItem()
{
}

BaseModelItem *GetModelItem::createChild( FTL::CStrRef portName )
{
  if ( portName == FTL_STR("varPath") )
    return pushChild(new VarPathModelItem(
      m_dfgUICmdHandler,
      m_binding,
      m_execPath,
      m_exec,
      m_nodeName
      ));
  else
    return pushChild(new GetPortModelItem(
      m_dfgUICmdHandler,
      m_binding,
      m_execPath,
      m_exec,
      m_nodeName,
      portName
      ));
}

} // namespace ModelItems
} // namespace FabricUI
