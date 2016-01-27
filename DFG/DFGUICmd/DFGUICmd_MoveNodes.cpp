//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_MoveNodes.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_MoveNodes::appendDesc( std::string &desc )
{
  desc += FTL_STR("Move ");
  appendDesc_NodeNames( m_nodeNames, desc );
}

void DFGUICmd_MoveNodes::invoke( unsigned &coreUndoCount )
{
  MoveNodes(
    getBinding(),
    getExecPath(),
    getExec(),
    m_nodeNames,
    m_newTopLeftPoss,
    coreUndoCount
    );
}

FABRIC_UI_DFG_NAMESPACE_END
