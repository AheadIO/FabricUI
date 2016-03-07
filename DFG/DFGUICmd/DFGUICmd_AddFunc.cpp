// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddFunc.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddFunc::appendDesc( QString &desc )
{
  desc += "Add function ";
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddFunc::invokeAdd( unsigned &coreUndoCount )
{
  return invokeAdd(
    getDesiredNodeName().toUtf8().constData(),
    m_initialCode.toUtf8().constData(),
    coreUndoCount
    );
}

FTL::CStrRef DFGUICmd_AddFunc::invokeAdd(
  FTL::CStrRef desiredNodeName,
  FTL::CStrRef initialCode,
  unsigned &coreUndoCount
  )
{
  FTL::CStrRef actualNodeName =
    getExec().addInstWithNewFunc( desiredNodeName.c_str() );
  ++coreUndoCount;

  FabricCore::DFGExec subExec =
    getExec().getSubExec( actualNodeName.c_str() );
  subExec.setCode( initialCode.c_str() );
  ++coreUndoCount;

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
