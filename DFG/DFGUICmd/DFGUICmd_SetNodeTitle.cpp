//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetNodeTitle.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetNodeTitle::appendDesc( std::string &desc )
{
  desc += FTL_STR("Change title of ");
  appendDesc_NodeName( m_nodeName, desc );
}

void DFGUICmd_SetNodeTitle::invoke( unsigned &coreUndoCount )
{
  switch ( getExec().getNodeType( m_nodeName.c_str() ) )
  {
    case FabricCore::DFGNodeType_User:
    {
      getExec().setNodeMetadata(
        m_nodeName.c_str(),
        "uiTitle",
        m_title.c_str(),
        true,
        true
        );
      ++coreUndoCount;
    }
    break;

    case FabricCore::DFGNodeType_Inst:
    {
      getExec().setInstTitle(
        m_nodeName.c_str(),
        m_title.c_str()
        );
      ++coreUndoCount;
    }
    break;

    default: break;
  }
}

FABRIC_UI_DFG_NAMESPACE_END
