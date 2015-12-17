//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SplitFromPreset.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SplitFromPreset::appendDesc( QString &desc )
{
  desc += "SplitFromPreset";
  QString execPath = getExecPath();
  if ( !execPath.isEmpty() )
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
