// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetTitle__
#define __UI_DFG_DFGUICmd_SetTitle__

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetTitle
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SetTitle(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef title
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_title( title )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetTitle"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_title;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetTitle__
