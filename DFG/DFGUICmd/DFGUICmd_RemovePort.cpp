//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_RemovePort.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_RemovePort::appendDesc( std::string &desc )
{
  desc += FTL_STR("Remove ");
  appendDesc_PortPath( m_portName, desc );
}

void DFGUICmd_RemovePort::invoke( unsigned &coreUndoCount )
{
  Perform(
    getBinding(),
    getExecPath(),
    getExec(),
    m_portName,
    coreUndoCount
    );
}

void DFGUICmd_RemovePort::Perform(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef portName,
  unsigned &coreUndoCount
  )
{
  exec.removeExecPort( portName.c_str() );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
