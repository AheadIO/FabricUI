//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_RemovePort.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_RemovePort::appendDesc( QString &desc )
{
  desc += "Remove ";
  appendDesc_PortPath( m_portName, desc );
}

void DFGUICmd_RemovePort::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_portName.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_RemovePort::invoke(
  FTL::CStrRef portName,
  unsigned &coreUndoCount
  )
{
  FabricCore::DFGExec &exec = getExec();

  exec.removeExecPort( portName.c_str() );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
