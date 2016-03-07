//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddVar.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddVar::appendDesc( QString &desc )
{
  desc += "Add variable ";
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddVar::invokeAdd( unsigned &coreUndoCount )
{
  return invokeAdd(
    getDesiredNodeName().toUtf8().constData(),
    m_dataType.toUtf8().constData(),
    m_extDep.toUtf8().constData(),
    coreUndoCount
    );
}

FTL::CStrRef DFGUICmd_AddVar::invokeAdd(
  FTL::CStrRef desiredNodeName,
  FTL::CStrRef dataType,
  FTL::CStrRef extDep,
  unsigned &coreUndoCount
  )
{
  FTL::CStrRef actualNodeName =
    getExec().addVar(
      desiredNodeName.c_str(),
      dataType.c_str(),
      extDep.c_str()
      );
  ++coreUndoCount;

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
