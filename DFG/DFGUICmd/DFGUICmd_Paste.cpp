//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Paste.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_Paste::appendDesc( QString &desc )
{
  desc += "Paste JSON as ";
  appendDesc_NodeNames( m_pastedNodeNames, desc );
}

void DFGUICmd_Paste::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_json.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_Paste::invoke(
  FTL::CStrRef json,
  unsigned &coreUndoCount
  )
{
  FabricCore::String newNodeNamesJSON =
    getExec().importNodesJSON( json.c_str() );
  ++coreUndoCount;

  m_pastedNodeNames =
    adjustNewNodes(
      newNodeNamesJSON,
      m_cursorPos,
      coreUndoCount
      );
}

FABRIC_UI_DFG_NAMESPACE_END
