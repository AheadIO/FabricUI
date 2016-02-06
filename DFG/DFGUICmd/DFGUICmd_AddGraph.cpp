// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

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
    getExec().addInstWithNewGraph( getDesiredNodeName().c_str() );
  ++coreUndoCount;

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
