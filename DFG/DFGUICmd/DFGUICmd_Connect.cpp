//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Connect.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_Connect::appendDesc( QString &desc )
{
  desc += "Connect ";
  appendDesc_Path( m_srcPath, desc );
  desc += " to ";
  appendDesc_Path( m_dstPath, desc );
}

void DFGUICmd_Connect::invoke( unsigned &coreUndoCount )
{
  invokeAdd(
    m_srcPath.toUtf8().constData(),
    m_dstPath.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_Connect::invokeAdd(
  FTL::CStrRef srcPath,
  FTL::CStrRef dstPath,
  unsigned &coreUndoCount
  )
{
  getExec().connectTo(
    srcPath.c_str(),
    dstPath.c_str()
    );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
