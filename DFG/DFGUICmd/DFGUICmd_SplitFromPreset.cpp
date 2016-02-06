//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SplitFromPreset.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SplitFromPreset::appendDesc( std::string &desc )
{
  desc += FTL_STR("SplitFromPreset");
  FTL::StrRef execPath = getExecPath();
  if ( !execPath.empty() )
  {
    desc += ' ';
    desc += execPath;
  }
}

void DFGUICmd_SplitFromPreset::invoke( unsigned &coreUndoCount )
{
  getExec().maybeSplitFromPreset();
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
