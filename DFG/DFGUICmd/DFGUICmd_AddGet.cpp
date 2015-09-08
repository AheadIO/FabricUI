// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddGet.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddGet::appendDesc( std::string &desc )
{
  desc += FTL_STR("Add get ");
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddGet::invokeAdd( unsigned &coreUndoCount )
{
  FTL::CStrRef actualNodeName =
    getExec().addGet(
      getDesiredNodeName().c_str(),
      m_varPath.c_str()
      );
  ++coreUndoCount;

  getBinding().execute();

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
