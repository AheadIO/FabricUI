// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetRefVarPath__
#define __UI_DFG_DFGUICmd_SetRefVarPath__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetRefVarPath
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SetRefVarPath(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString refName,
    QString varPath
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_refName( refName )
    , m_varPath( varPath )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetRefVarPath"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invoke(
    FTL::CStrRef refName,
    FTL::CStrRef varPath,
    unsigned &coreUndoCount
    );

private:

  QString m_refName;
  QString m_varPath;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetRefVarPath__
