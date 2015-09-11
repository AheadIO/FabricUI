//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetNodeComment.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetNodeComment::appendDesc( std::string &desc )
{
  desc += FTL_STR("Change comment of ");
  appendDesc_NodeName( m_nodeName, desc );
}

void DFGUICmd_SetNodeComment::invoke( unsigned &coreUndoCount )
{
  getExec().setNodeMetadata(
    m_nodeName.c_str(),
    "uiComment",
    m_comment.c_str(),
    true,
    true
    );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
