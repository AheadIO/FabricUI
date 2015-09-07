// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetNodeTitle__
#define __UI_DFG_DFGUICmd_SetNodeTitle__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetNodeTitle
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SetNodeTitle(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef nodeName,
    FTL::StrRef title
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_nodeName( nodeName )
    , m_title( title )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetNodeTitle"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_nodeName;
  std::string m_title;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetNodeTitle__
