// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetCode__
#define __UI_DFG_DFGUICmd_SetCode__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetCode
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SetCode(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef code
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_code( code )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetCode"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_code;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetCode__
