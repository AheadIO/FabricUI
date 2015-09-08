// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_InstPreset.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_InstPreset::appendDesc( std::string &desc )
{
  desc += FTL_STR("Instance preset ");
  AppendDesc_String( getPresetPath(), desc );
  desc += FTL_STR(" as ");
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_InstPreset::invokeAdd( unsigned &coreUndoCount )
{
  FTL::CStrRef actualNodeName =
    getExec().addInstFromPreset( getPresetPath().c_str() );
  ++coreUndoCount;

  getBinding().execute();

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
