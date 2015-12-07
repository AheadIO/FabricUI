// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddPort__
#define __UI_DFG_DFGUICmd_AddPort__

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddPort
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_AddPort(
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef desiredPortName,
    FabricCore::DFGPortType portType,
    FTL::StrRef typeSpec,
    FTL::StrRef portToConnectWith,
    FTL::StrRef extDep,
    FTL::StrRef metaData
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_desiredPortName( desiredPortName )
    , m_portType( portType )
    , m_typeSpec( typeSpec )
    , m_portToConnectWith( portToConnectWith )
    , m_extDep( extDep )
    , m_metaData( metaData )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("AddPort"); }

  FTL::CStrRef getActualPortName()
  {
    assert( wasInvoked() );
    return m_actualPortName;
  }

protected:

  virtual void appendDesc( std::string &desc );

  virtual void invoke( unsigned &coreUndoCount );

  static FTL::CStrRef Perform(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredPortName,
    FabricCore::DFGPortType portType,
    FTL::CStrRef typeSpec,
    FTL::CStrRef portToConnect,
    FTL::CStrRef extDep,
    FTL::CStrRef metaData,
    unsigned &coreUndoCount
    );

private:

  std::string m_desiredPortName;
  FabricCore::DFGPortType m_portType;
  std::string m_typeSpec;
  std::string m_portToConnectWith;
  std::string m_extDep;
  std::string m_metaData;
  
  std::string m_actualPortName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddPort__
