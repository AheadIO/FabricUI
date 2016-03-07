//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_RenamePort.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_RenamePort::appendDesc( QString &desc )
{
  desc += "Rename ";
  appendDesc_PortPath( m_oldPortName, desc );
  desc += " to ";
  appendDesc_PortPath( m_actualNewPortName, desc );
}

void DFGUICmd_RenamePort::invoke( unsigned &coreUndoCount )
{
  FTL::CStrRef actualNewPortName =
    invoke(
      m_oldPortName.toUtf8().constData(),
      m_desiredNewPortName.toUtf8().constData(),
      coreUndoCount
      );
  m_actualNewPortName = QString::fromUtf8(
    actualNewPortName.data(), actualNewPortName.size()
    );
}

FTL::CStrRef DFGUICmd_RenamePort::invoke(
  FTL::CStrRef oldPortName,
  FTL::CStrRef desiredNewPortName,
  unsigned &coreUndoCount
  )
{
  FTL::CStrRef actualNewPortName =
    getExec().renameExecPort(
      oldPortName.c_str(),
      desiredNewPortName.c_str()
      );
  ++coreUndoCount;

  return actualNewPortName;
}

FABRIC_UI_DFG_NAMESPACE_END
