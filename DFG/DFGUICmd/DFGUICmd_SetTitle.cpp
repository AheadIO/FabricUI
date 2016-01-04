//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetTitle.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetTitle::appendDesc( std::string &desc )
{
  desc += FTL_STR("Change title of executable ");
  AppendDesc_String( getExecPath(), desc );
}

void DFGUICmd_SetTitle::invoke( unsigned &coreUndoCount )
{
  FabricCore::DFGExec &exec = getExec();

  exec.setTitle( m_title.c_str() );
  ++coreUndoCount;

  FTL::CStrRef execPath = getExecPath();
  FTL::CStrRef::Split split = execPath.rsplit('.');
  if ( !split.second.empty() )
  {
    FabricCore::DFGBinding &binding = getBinding();
    FabricCore::DFGExec rootExec = binding.getExec();
    FabricCore::DFGExec parentExec =
      rootExec.getSubExec( std::string( split.first ).c_str() );
    parentExec.renameNode( split.second.c_str(), m_title.c_str() );
    ++coreUndoCount;
  }
}

FABRIC_UI_DFG_NAMESPACE_END
