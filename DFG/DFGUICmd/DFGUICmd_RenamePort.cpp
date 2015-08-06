//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_RenamePort.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_RenamePort::appendDesc( std::string &desc )
{
  desc += FTL_STR("Rename ");
  appendDesc_PortPath( m_actualNewPortName, desc );
  desc += FTL_STR(" to ");
  appendDesc_PortPath( m_oldPortName, desc );
}

void DFGUICmd_RenamePort::invoke( unsigned &coreUndoCount )
{
  m_actualNewPortName =
    getExec().renameExecPort(
      m_oldPortName.c_str(),
      m_desiredNewPortName.c_str()
      );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
