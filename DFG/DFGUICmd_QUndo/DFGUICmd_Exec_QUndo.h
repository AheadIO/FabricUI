// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_Exec_QUndo__
#define __UI_DFG_DFGUICmd_Exec_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Binding_QUndo.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_Exec_QUndo : public DFGUICmd_Binding_QUndo
{
public:

  DFGUICmd_Exec_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec
    )
    : DFGUICmd_Binding_QUndo(
      desc,
      binding
      )
    , m_execPath( execPath )
    , m_exec( exec )
    {}

protected:

  virtual void invokeForBinding(
    FabricCore::DFGBinding &binding,
    unsigned &coreUndoCount
    )
  {
    invokeForExec( binding, m_execPath, m_exec, coreUndoCount );
  }

  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    ) = 0;

private:

  FTL::CStrRef m_execPath;
  FabricCore::DFGExec m_exec;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_Exec_QUndo__
