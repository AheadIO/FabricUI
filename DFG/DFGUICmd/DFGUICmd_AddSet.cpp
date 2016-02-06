//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddSet.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddSet::appendDesc( std::string &desc )
{
  desc += FTL_STR("Add set ");
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_AddSet::invokeAdd( unsigned &coreUndoCount )
{
  FTL::CStrRef actualNodeName =
    getExec().addSet(
      getDesiredNodeName().c_str(),
      m_varPath.c_str()
      );
  ++coreUndoCount;
  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
