// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_RenamePort__
#define __UI_DFG_DFGUICmd_RenamePort__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_RenamePort
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_RenamePort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString oldPortName,
    QString desiredNewPortName
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_oldPortName( oldPortName.trimmed() )
    , m_desiredNewPortName( desiredNewPortName.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("RenamePort"); }

  QString getActualNewPortName()
  {
    assert( wasInvoked() );
    return m_actualNewPortName;
  }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  FTL::CStrRef invoke(
    FTL::CStrRef oldPortName,
    FTL::CStrRef desiredNewPortName,
    unsigned &coreUndoCount
    );

private:

  QString m_oldPortName;
  QString m_desiredNewPortName;

  QString m_actualNewPortName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RenamePort__
