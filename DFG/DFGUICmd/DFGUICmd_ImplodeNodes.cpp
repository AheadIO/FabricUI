//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_ImplodeNodes.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_ImplodeNodes::appendDesc( QString &desc )
{
  desc += "Implode ";
  appendDesc_NodeName( m_actualImplodedNodeName, desc );
  desc += " from ";
  appendDesc_NodeNames( m_nodeNames, desc );
}

void DFGUICmd_ImplodeNodes::invoke( unsigned &coreUndoCount )
{
  QList<QByteArray> nodeNameBAs;
#if QT_VERSION >= 0x040800
  nodeNameBAs.reserve( m_nodeNames.size() );
#endif
  foreach ( QString nodeName, m_nodeNames )
    nodeNameBAs.push_back( nodeName.toUtf8() );

  std::vector<FTL::CStrRef> nodeNames;
  nodeNames.reserve( m_nodeNames.size() );
  foreach ( QByteArray nodeNameBA, nodeNameBAs )
    nodeNames.push_back( nodeNameBA.constData() );

  FTL::CStrRef actualImplodedNodeName =
    invoke(
      nodeNames,
      m_desiredImplodedNodeName.toUtf8().constData(),
      coreUndoCount
      );
  m_actualImplodedNodeName = QString::fromUtf8(
    actualImplodedNodeName.data(), actualImplodedNodeName.size()
    );
}

FTL::CStrRef DFGUICmd_ImplodeNodes::invoke(
  FTL::ArrayRef<FTL::CStrRef> nodeNames,
  FTL::CStrRef desiredImplodedNodeName,
  unsigned &coreUndoCount
  )
{
  FabricCore::DFGExec &exec = getExec();

  size_t count = nodeNames.size();

  QPointF uiGraphPos;
  if ( count > 0 )
  {
    for ( size_t i = 0; i < count; ++i )
      uiGraphPos += getNodeUIGraphPos( nodeNames[i] );
    uiGraphPos /= count;
  }

  std::vector<char const *> nodeNameCStrs;
  nodeNameCStrs.reserve( count );
  for ( size_t i = 0; i < count; ++i )
    nodeNameCStrs.push_back( nodeNames[i].c_str() );

  FTL::CStrRef actualImplodedNodeName =
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
  {
    moveNodes(
      actualImplodedNodeName,
      uiGraphPos,
      coreUndoCount
      );
  }

  return actualImplodedNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
