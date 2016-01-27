// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SplitFromPreset__
#define __UI_DFG_DFGUICmd_SplitFromPreset__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SplitFromPreset
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SplitFromPreset(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SplitFromPreset"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SplitFromPreset__
