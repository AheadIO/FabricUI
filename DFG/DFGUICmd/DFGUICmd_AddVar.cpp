//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddVar.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddVar::appendDesc( std::string &desc )
{
  desc += FTL_STR("Add variable ");
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddVar::invokeAdd( unsigned &coreUndoCount )
{
  FTL::CStrRef actualNodeName =
    getExec().addVar(
      getDesiredNodeName().c_str(),
      m_dataType.c_str(),
      m_extDep.c_str()
      );
  ++coreUndoCount;

  getBinding().execute();

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
