// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddBackDrop.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddBackDrop::appendDesc( std::string &desc )
{
  desc += FTL_STR("Add backdrop ");
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddBackDrop::invokeAdd( unsigned &coreUndoCount )
{
  FTL::CStrRef actualNodeName =
    getExec().addUser( "backDrop" );
  ++coreUndoCount;

  getExec().setNodeMetadata(
    actualNodeName.c_str(),
    "uiTitle",
    getText().c_str(),
    true,
    true
    );
  ++coreUndoCount;

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
