// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddGraph.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddGraph::appendDesc( std::string &desc )
{
  desc += FTL_STR("Add graph ");
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddGraph::invokeAdd( unsigned &coreUndoCount )
{
  FTL::CStrRef actualNodeName =
    getExec().addInstWithNewGraph( getTitle().c_str() );
  ++coreUndoCount;

  getBinding().execute();
  
  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
