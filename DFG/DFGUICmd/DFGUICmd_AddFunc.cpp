// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddFunc.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddFunc::appendDesc( std::string &desc )
{
  desc += FTL_STR("Add function ");
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddFunc::invokeAdd( unsigned &coreUndoCount )
{
  FTL::CStrRef actualNodeName =
    getExec().addInstWithNewFunc( getTitle().c_str() );
  ++coreUndoCount;

  FabricCore::DFGExec subExec =
    getExec().getSubExec( actualNodeName.c_str() );
  subExec.setCode( m_initialCode.c_str() );
  ++coreUndoCount;

  getBinding().execute();
  
  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
