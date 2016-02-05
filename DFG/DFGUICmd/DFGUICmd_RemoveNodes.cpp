//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_RemoveNodes.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_RemoveNodes::appendDesc( std::string &desc )
{
  desc += FTL_STR("Remove ");
  appendDesc_NodeNames( m_nodeNames, desc );
}

void DFGUICmd_RemoveNodes::invoke( unsigned &coreUndoCount )
{
  for ( size_t i = 0; i < m_nodeNames.size(); ++i )
  {
    getExec().removeNode( m_nodeNames[i].c_str() );
    ++coreUndoCount;
  }
}

FABRIC_UI_DFG_NAMESPACE_END
