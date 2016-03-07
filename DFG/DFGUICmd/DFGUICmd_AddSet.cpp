//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddSet.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddSet::appendDesc( QString &desc )
{
  desc += "Add set ";
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddSet::invokeAdd( unsigned &coreUndoCount )
{
  return invokeAdd(
    getDesiredNodeName().toUtf8().constData(),
    m_varPath.toUtf8().constData(),
    coreUndoCount
    );
}

FTL::CStrRef DFGUICmd_AddSet::invokeAdd(
  FTL::CStrRef desiredNodeName,
  FTL::CStrRef varPath,
  unsigned &coreUndoCount
  )
{
  FTL::CStrRef actualNodeName =
    getExec().addSet(
      desiredNodeName.c_str(),
      varPath.c_str()
      );
  ++coreUndoCount;
  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
