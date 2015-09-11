// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_EditPort__
#define __UI_DFG_DFGUICmd_EditPort__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_EditPort
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_EditPort(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef oldPortName,
    FTL::StrRef desiredNewPortName,
    FTL::StrRef typeSpec,
    FTL::StrRef extDep,
    FTL::StrRef uiMetadata
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_oldPortName( oldPortName.trim() )
    , m_desiredNewPortName( desiredNewPortName.trim() )
    , m_typeSpec( typeSpec.trim() )
    , m_extDep( extDep.trim() )
    , m_uiMetadata( uiMetadata.trim() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("EditPort"); }

  FTL::CStrRef getActualNewPortName()
  {
    assert( wasInvoked() );
    return m_actualNewPortName;
  }

protected:

  virtual void appendDesc( std::string &desc );

  virtual void invoke( unsigned &coreUndoCount );

  static FTL::CStrRef Perform(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef oldPortName,
    FTL::CStrRef desiredNewPortName,
    FTL::CStrRef typeSpec,
    FTL::CStrRef extDep,
    FTL::CStrRef uiMetadata,
    unsigned &coreUndoCount
    );

private:

  std::string m_oldPortName;
  std::string m_desiredNewPortName;
  std::string m_typeSpec;
  std::string m_extDep;
  std::string m_uiMetadata;
  
  std::string m_actualNewPortName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_EditPort__
