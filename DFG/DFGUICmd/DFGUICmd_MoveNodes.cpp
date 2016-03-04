//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_MoveNodes.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_MoveNodes::appendDesc( QString &desc )
{
  desc += "Move ";
  appendDesc_NodeNames( m_nodeNames, desc );
}

void DFGUICmd_MoveNodes::invoke( unsigned &coreUndoCount )
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

  std::vector<QPointF> newTopLeftPoss;
  newTopLeftPoss.reserve( m_newTopLeftPoss.size() );
  foreach ( QPointF newTopLeftPos, m_newTopLeftPoss )
    newTopLeftPoss.push_back( newTopLeftPos );

  moveNodes(
    nodeNames,
    newTopLeftPoss,
    coreUndoCount
    );
}

FABRIC_UI_DFG_NAMESPACE_END
