//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetRefVarPath.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetRefVarPath::appendDesc( std::string &desc )
{
  desc += FTL_STR("Set variable for reference ");
  appendDesc_NodeName( m_refName, desc );
  desc += FTL_STR(" to ");
  AppendDesc_String( m_varPath, desc );
}

void DFGUICmd_SetRefVarPath::invoke( unsigned &coreUndoCount )
{
  getExec().setRefVarPath( m_refName.c_str(), m_varPath.c_str(), true );
  ++coreUndoCount;

  getBinding().execute();
}

FABRIC_UI_DFG_NAMESPACE_END
