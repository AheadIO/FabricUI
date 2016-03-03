//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_ImplodeNodes.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_ImplodeNodes::appendDesc( std::string &desc )
{
  desc += FTL_STR("Implode ");
  appendDesc_NodeName( m_actualImplodedNodeName, desc );
  desc += FTL_STR(" from ");
  appendDesc_NodeNames( m_nodeNames, desc );
}

void DFGUICmd_ImplodeNodes::invoke( unsigned &coreUndoCount )
{
  m_actualImplodedNodeName =
    Perform(
      getBinding(),
      getExecPath(),
      getExec(),
      m_nodeNames,
      m_desiredImplodedNodeName,
      coreUndoCount
      );
}

std::string DFGUICmd_ImplodeNodes::Perform(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::ArrayRef<std::string> nodeNames,
  FTL::CStrRef desiredImplodedNodeName,
  unsigned &coreUndoCount
  )
{
  size_t count = nodeNames.size();

  QPointF uiGraphPos;
  if ( count > 0 )
  {
    for ( size_t i = 0; i < count; ++i )
      uiGraphPos += GetNodeUIGraphPos( exec, nodeNames[i] );
    uiGraphPos /= count;
  }

  std::vector<char const *> nodeNameCStrs;
  nodeNameCStrs.reserve( count );
  for ( size_t i = 0; i < count; ++i )
    nodeNameCStrs.push_back( nodeNames[i].c_str() );

  std::string actualImplodedNodeName =
    exec.implodeNodes(
      desiredImplodedNodeName.c_str(),
      nodeNameCStrs.size(),
      &nodeNameCStrs[0]
      );
  ++coreUndoCount;

  exec.setInstTitle(
    actualImplodedNodeName.c_str(),
    desiredImplodedNodeName.c_str()
    );
  ++coreUndoCount;

  if ( count > 0 )
    MoveNodes(
      binding,
      execPath,
      exec,
      actualImplodedNodeName,
      uiGraphPos,
      coreUndoCount
      );

  return actualImplodedNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
