// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetExtDeps__
#define __UI_DFG_DFGUICmd_SetExtDeps__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetExtDeps
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_SetExtDeps(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList extDeps
    )
    : DFGUICmd_Exec( binding, execPath, exec )
    , m_extDeps( extDeps )
  {
  }

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("SetExtDeps"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invoke(
    FTL::ArrayRef<char const *> extDepCStrs,
    unsigned &coreUndoCount
    );

private:

  QStringList m_extDeps;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetExtDeps__
