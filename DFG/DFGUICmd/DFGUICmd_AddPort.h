// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddPort__
#define __UI_DFG_DFGUICmd_AddPort__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddPort
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_AddPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredPortName,
    FabricCore::DFGPortType portType,
    QString typeSpec,
    QString portToConnectWith,
    QString extDep,
    QString metaData
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_desiredPortName( desiredPortName.trimmed() )
    , m_portType( portType )
    , m_typeSpec( typeSpec.trimmed() )
    , m_portToConnectWith( portToConnectWith.trimmed() )
    , m_extDep( extDep.trimmed() )
    , m_metaData( metaData.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("AddPort"); }

  QString getActualPortName()
  {
    assert( wasInvoked() );
    return m_actualPortName;
  }

protected:

  virtual void appendDesc( QString &desc );

  virtual void invoke( unsigned &coreUndoCount );

  FTL::CStrRef invoke(
    FTL::CStrRef execPath,
    FTL::CStrRef desiredPortName,
    FTL::CStrRef typeSpec,
    FTL::CStrRef portToConnect,
    FTL::CStrRef extDep,
    FTL::CStrRef metaData,
    unsigned &coreUndoCount
    );

private:

  QString m_desiredPortName;
  FabricCore::DFGPortType m_portType;
  QString m_typeSpec;
  QString m_portToConnectWith;
  QString m_extDep;
  QString m_metaData;
  
  QString m_actualPortName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddPort__
