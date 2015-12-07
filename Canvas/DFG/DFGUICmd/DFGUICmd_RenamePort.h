// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_RenamePort__
#define __UI_DFG_DFGUICmd_RenamePort__

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_RenamePort
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_RenamePort(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef oldPortName,
    FTL::StrRef desiredNewPortName
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_oldPortName( oldPortName )
    , m_desiredNewPortName( desiredNewPortName )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("RenamePort"); }

  FTL::CStrRef getActualNewPortName()
  {
    assert( wasInvoked() );
    return m_actualNewPortName;
  }

protected:
  
  virtual void appendDesc( std::string &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  std::string m_oldPortName;
  std::string m_desiredNewPortName;

  std::string m_actualNewPortName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RenamePort__
