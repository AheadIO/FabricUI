//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_ExplodeNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_ExplodeNode::appendDesc( std::string &desc )
{
  desc += FTL_STR("Explode ");
  appendDesc_NodeName( m_nodeName, desc );
  desc += FTL_STR(" to ");
  appendDesc_NodeNames( m_explodedNodeNames, desc );
}

void DFGUICmd_ExplodeNode::invoke( unsigned &coreUndoCount )
{
  m_explodedNodeNames =
    Perform(
      getBinding(),
      getExecPath(),
      getExec(),
      m_nodeName,
      coreUndoCount
      );
}

std::vector<std::string> DFGUICmd_ExplodeNode::Perform(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  unsigned &coreUndoCount
  )
{
  QPointF oldTopLeftPos = GetNodeUIGraphPos( exec, nodeName );

  FabricCore::DFGStringResult newNodeNamesJSON =
    exec.explodeNode( nodeName.c_str() );
  ++coreUndoCount;

  return adjustNewNodes(
    newNodeNamesJSON,
    oldTopLeftPos,
    coreUndoCount
    );
}

FABRIC_UI_DFG_NAMESPACE_END
