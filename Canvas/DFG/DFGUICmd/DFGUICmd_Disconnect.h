// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_Disconnect__
#define __UI_DFG_DFGUICmd_Disconnect__

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_Disconnect
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_Disconnect(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef srcPath,
    FTL::StrRef dstPath
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_srcPath( srcPath )
    , m_dstPath( dstPath )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("Disconnect"); }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_srcPath;
  std::string m_dstPath;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_Disconnect__
