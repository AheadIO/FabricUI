//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/SetModelItem.h>
#include <FabricUI/ModelItems/SetPortModelItem.h>
#include <FabricUI/ModelItems/VarPathModelItem.h>

namespace FabricUI {
namespace ModelItems {

SetModelItem::SetModelItem(
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

SetModelItem::~SetModelItem()
{
}

BaseModelItem *SetModelItem::createChild( FTL::CStrRef portName )
{
  if ( portName == FTL_STR("varPath") )
    return new VarPathModelItem(
      m_dfgUICmdHandler,
      m_binding,
      m_execPath,
      m_exec,
      m_nodeName
      );
  else
    return new SetPortModelItem(
      m_dfgUICmdHandler,
      m_binding,
      m_execPath,
      m_exec,
      m_nodeName,
      portName
      );
}

} // namespace ModelItems
} // namespace FabricUI
