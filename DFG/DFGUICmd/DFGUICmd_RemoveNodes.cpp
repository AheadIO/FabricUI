//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_RemoveNodes.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_RemoveNodes::appendDesc( QString &desc )
{
  desc += "Remove ";
  appendDesc_NodeNames( m_nodeNames, desc );
}

void DFGUICmd_RemoveNodes::invoke( unsigned &coreUndoCount )
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

  return invoke(
    nodeNames,
    coreUndoCount
    );
}

void DFGUICmd_RemoveNodes::invoke(
  FTL::ArrayRef<FTL::CStrRef> nodeNames,
  unsigned &coreUndoCount
  )
{
  for ( size_t i = 0; i < nodeNames.size(); ++i )
  {
    getExec().removeNode( nodeNames[i].c_str() );
    ++coreUndoCount;
  }
}

FABRIC_UI_DFG_NAMESPACE_END
