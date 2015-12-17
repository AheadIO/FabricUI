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
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString oldPortName,
    QString desiredNewPortName,
    QString typeSpec,
    QString extDep,
    QString uiMetadata
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_oldPortName( oldPortName.trimmed() )
    , m_desiredNewPortName( desiredNewPortName.trimmed() )
    , m_typeSpec( typeSpec.trimmed() )
    , m_extDep( extDep.trimmed() )
    , m_uiMetadata( uiMetadata.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("EditPort"); }

  QString getActualNewPortName()
  {
    assert( wasInvoked() );
    return m_actualNewPortName;
  }

protected:

  virtual void appendDesc( QString &desc );

  virtual void invoke( unsigned &coreUndoCount );

  FTL::CStrRef invoke(
    FTL::CStrRef execPath,
    FTL::CStrRef oldPortName,
    FTL::CStrRef desiredNewPortName,
    FTL::CStrRef typeSpec,
    FTL::CStrRef extDep,
    FTL::CStrRef uiMetadata,
    unsigned &coreUndoCount
    );

private:

  QString m_oldPortName;
  QString m_desiredNewPortName;
  QString m_typeSpec;
  QString m_extDep;
  QString m_uiMetadata;
  
  QString m_actualNewPortName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_EditPort__
