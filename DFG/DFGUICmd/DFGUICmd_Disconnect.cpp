//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Disconnect.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_Disconnect::appendDesc( QString &desc )
{
  desc += "Disconnect ";
  appendDesc_Path( m_srcPath, desc );
  desc += " from ";
  appendDesc_Path( m_dstPath, desc );
}

void DFGUICmd_Disconnect::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_srcPath.toUtf8().constData(),
    m_dstPath.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_Disconnect::invoke(
  FTL::CStrRef srcPath,
  FTL::CStrRef dstPath,
  unsigned &coreUndoCount
  )
{
  getExec().disconnectFrom(
    srcPath.c_str(),
    dstPath.c_str()
    );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
