//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_SetArgType.h>
#include <FabricUI/Canvas/DFG/DFGUIUtil.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetArgType::appendDesc( std::string &desc )
{
  desc += FTL_STR("Set type of argument ");
  appendDesc_ArgName( m_argName, desc );
  desc += FTL_STR(" to ");
  AppendDesc_String( m_typeName, desc );
}

void DFGUICmd_SetArgType::invoke( unsigned &coreUndoCount )
{
  FabricCore::DFGBinding &binding = getBinding();
  FabricCore::DFGHost host = binding.getHost();
  FabricCore::Context context = host.getContext();
  FabricCore::RTVal value = DFGCreateDefaultValue( context, m_typeName );
  binding.setArgValue( m_argName.c_str(), value, true );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
