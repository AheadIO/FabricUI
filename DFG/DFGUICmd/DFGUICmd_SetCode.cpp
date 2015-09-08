//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetCode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetCode::appendDesc( std::string &desc )
{
  desc += FTL_STR("Modify code of function ");
  AppendDesc_String( getExecPath(), desc );
}

void DFGUICmd_SetCode::invoke( unsigned &coreUndoCount )
{
  getExec().setCode( m_code.c_str() );
  ++coreUndoCount;

  getBinding().execute();
}

FABRIC_UI_DFG_NAMESPACE_END
