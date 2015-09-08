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
    getExec().addUser( getTitle().c_str() );
  ++coreUndoCount;

  getExec().setNodeMetadata(
    actualNodeName.c_str(),
    "uiTitle",
    getTitle().c_str(),
    true
    );
  ++coreUndoCount;

  getBinding().execute();

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
