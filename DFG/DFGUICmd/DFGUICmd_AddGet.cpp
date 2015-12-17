// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddGet.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddGet::appendDesc( QString &desc )
{
  desc += "Add get ";
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddGet::invokeAdd( unsigned &coreUndoCount )
{
  return invokeAdd(
    getDesiredNodeName().toUtf8().constData(),
    m_varPath.toUtf8().constData(),
    coreUndoCount
    );
}

FTL::CStrRef DFGUICmd_AddGet::invokeAdd(
  FTL::CStrRef desiredNodeName,
  FTL::CStrRef varPath,
  unsigned &coreUndoCount
  )
{
  FTL::CStrRef actualNodeName =
    getExec().addGet(
      desiredNodeName.c_str(),
      varPath.c_str()
      );
  ++coreUndoCount;

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
