//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetNodeComment.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetNodeComment::appendDesc( QString &desc )
{
  desc += "Change comment of ";
  appendDesc_NodeName( m_nodeName, desc );
}

void DFGUICmd_SetNodeComment::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_nodeName.toUtf8().constData(),
    m_comment.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_SetNodeComment::invoke(
  FTL::CStrRef nodeName,
  FTL::CStrRef comment,
  unsigned &coreUndoCount
  )
{
  getExec().setNodeMetadata(
    nodeName.c_str(),
    "uiComment",
    comment.c_str(),
    true,
    true
    );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
