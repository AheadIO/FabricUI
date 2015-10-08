//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_RenameNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_RenameNode::appendDesc( std::string &desc )
{
  desc += FTL_STR("Rename ");
  appendDesc_NodeName( m_oldNodeName, desc );
  desc += FTL_STR(" to ");
  appendDesc_NodeName( m_actualNewNodeName, desc );
}

void DFGUICmd_RenameNode::invoke( unsigned &coreUndoCount )
{
  FabricCore::DFGExec &exec = getExec();

  switch ( exec.getNodeType( m_oldNodeName.c_str() ) )
  {
    case FabricCore::DFGNodeType_User:
    {
      exec.setNodeMetadata(
        m_oldNodeName.c_str(),
        "uiTitle",
        m_desiredNewNodeName.c_str(),
        true,
        true
        );
      ++coreUndoCount;
    }
    break;

    case FabricCore::DFGNodeType_Inst:
    {
      FabricCore::DFGExec instExec =
        exec.getSubExec( m_oldNodeName.c_str() );
      if ( !instExec.editWouldSplitFromPreset() )
      {
        instExec.setTitle( m_desiredNewNodeName.c_str() );
        ++coreUndoCount;
      }
    }
    break;

    default: break;
  }

  m_actualNewNodeName = exec.renameNode(
    m_oldNodeName.c_str(),
    m_desiredNewNodeName.c_str()
    );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
