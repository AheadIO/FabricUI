//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_ExplodeNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_ExplodeNode::appendDesc( QString &desc )
{
  desc += "Explode ";
  appendDesc_NodeName( m_nodeName, desc );
  desc += " to ";
  appendDesc_NodeNames( m_explodedNodeNames, desc );
}

void DFGUICmd_ExplodeNode::invoke( unsigned &coreUndoCount )
{
  m_explodedNodeNames =
    invoke(
      m_nodeName.toUtf8().constData(),
      coreUndoCount
      );
}

QStringList DFGUICmd_ExplodeNode::invoke(
  FTL::CStrRef nodeName,
  unsigned &coreUndoCount
  )
{
  FabricCore::DFGExec &exec = getExec();

  QPointF oldTopLeftPos = getNodeUIGraphPos( nodeName );

  FabricCore::String newNodeNamesJSON =
    exec.explodeNode( nodeName.c_str() );
  ++coreUndoCount;

  return adjustNewNodes(
    newNodeNamesJSON,
    oldTopLeftPos,
    coreUndoCount
    );
}

FABRIC_UI_DFG_NAMESPACE_END
