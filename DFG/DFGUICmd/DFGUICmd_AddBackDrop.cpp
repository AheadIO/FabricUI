// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddBackDrop.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddBackDrop::appendDesc( QString &desc )
{
  desc += "Add backdrop ";
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddBackDrop::invokeAdd( unsigned &coreUndoCount )
{
  return invokeAdd(
    getText().toUtf8().constData(),
    coreUndoCount
    );
}

FTL::CStrRef DFGUICmd_AddBackDrop::invokeAdd(
  FTL::CStrRef text,
  unsigned &coreUndoCount
  )
{
  FTL::CStrRef actualNodeName =
    getExec().addUser( "backDrop" );
  ++coreUndoCount;

  getExec().setNodeMetadata(
    actualNodeName.c_str(),
    "uiTitle",
    text.c_str(),
    true,
    true
    );
  ++coreUndoCount;

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
