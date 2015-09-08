//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetPortDefaultValue.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetPortDefaultValue::appendDesc( std::string &desc )
{
  desc += FTL_STR("Set value of ");
  appendDesc_PortPath( m_portPath, desc );
}

void DFGUICmd_SetPortDefaultValue::invoke( unsigned &coreUndoCount )
{
  getExec().setPortDefaultValue( m_portPath.c_str(), m_value, true );
  ++coreUndoCount;

  getBinding().execute();
}

FABRIC_UI_DFG_NAMESPACE_END
