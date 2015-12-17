// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddGraph.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddGraph::appendDesc( QString &desc )
{
  desc += "Add graph ";
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddGraph::invokeAdd( unsigned &coreUndoCount )
{
  return invokeAdd(
    getDesiredNodeName().toUtf8().constData(),
    coreUndoCount
    );
}

FTL::CStrRef DFGUICmd_AddGraph::invokeAdd(
  FTL::CStrRef desiredNodeName,
  unsigned &coreUndoCount
  )
{
  FTL::CStrRef actualNodeName =
    getExec().addInstWithNewGraph( desiredNodeName.c_str() );
  ++coreUndoCount;

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
