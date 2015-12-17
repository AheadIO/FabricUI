// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_InstPreset.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_InstPreset::appendDesc( QString &desc )
{
  desc += "Instance preset ";
  AppendDesc_String( getPresetPath(), desc );
  desc += " as ";
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_InstPreset::invokeAdd( unsigned &coreUndoCount )
{
  return invokeAdd(
    getPresetPath().toUtf8().constData(),
    coreUndoCount
    );
}

FTL::CStrRef DFGUICmd_InstPreset::invokeAdd(
  FTL::CStrRef presetPath,
  unsigned &coreUndoCount
  )
{
  std::string desiredNodeName = presetPath.rsplit('.').second;
  desiredNodeName += FTL_STR("_1");

  FTL::CStrRef actualNodeName =
    getExec().addInstFromPreset(
      presetPath.c_str(),
      desiredNodeName.c_str()
      );
  ++coreUndoCount;

  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
