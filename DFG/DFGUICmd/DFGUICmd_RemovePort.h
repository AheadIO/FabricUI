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
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString portName
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_portName( portName.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("RemovePort"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invoke(
    FTL::CStrRef portName,
    unsigned &coreUndoCount
    );

private:

  QString m_portName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RemovePort__
