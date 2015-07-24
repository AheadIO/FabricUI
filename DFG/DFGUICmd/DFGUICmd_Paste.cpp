//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Paste.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_Paste::appendDesc( std::string &desc )
{
  desc += FTL_STR("Paste JSON as ");
  appendDesc_NodeNames( m_pastedNodeNames, desc );
}

void DFGUICmd_Paste::invoke( unsigned &coreUndoCount )
{
  FabricCore::DFGStringResult newNodeNamesJSON =
    getExec().importNodesJSON( m_json.c_str() );
  ++coreUndoCount;

  m_pastedNodeNames =
    adjustNewNodes(
      newNodeNamesJSON,
      m_cursorPos,
      coreUndoCount
      );
}

FABRIC_UI_DFG_NAMESPACE_END
