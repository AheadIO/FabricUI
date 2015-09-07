// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_RemovePort__
#define __UI_DFG_DFGUICmd_RemovePort__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_RemovePort
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_RemovePort(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef portName
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_portName( portName )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("RemovePort"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  static void Perform(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef portName,
    unsigned &coreUndoCount
    );

private:

  std::string m_portName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RemovePort__
