//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/SetPortItemMetadata.h>
#include <FabricUI/ModelItems/SetPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
SetPortModelItem::SetPortModelItem(
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

SetPortModelItem::~SetPortModelItem()
{
}

FTL::CStrRef SetPortModelItem::getName()
{
  return FTL_STR("value");
}

ItemMetadata* SetPortModelItem::getMetadata()
{
  if ( !m_metadata )
    m_metadata = new SetPortItemMetadata( this );
  return m_metadata;
}

} // namespace ModelItems
} // namespace FabricUI
