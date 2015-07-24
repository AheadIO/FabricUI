//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetArgValue.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetArgValue::appendDesc( std::string &desc )
{
  desc += FTL_STR("Set value of ");
  appendDesc_ArgName( m_argName, desc );
}

void DFGUICmd_SetArgValue::invoke( unsigned &coreUndoCount )
{
  getBinding().setArgValue( m_argName.c_str(), m_value, true );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
