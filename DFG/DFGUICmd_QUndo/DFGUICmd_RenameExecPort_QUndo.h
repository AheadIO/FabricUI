// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_RenameExecPort_QUndo__
#define __UI_DFG_DFGUICmd_RenameExecPort_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_RenameExecPort_QUndo
  : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_RenameExecPort_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef oldName,
    FTL::StrRef desiredNewName
    )
    : DFGUICmd_Exec_QUndo( desc, binding, execPath, exec )
    , m_oldName( oldName )
    , m_desiredNewName( desiredNewName )
  {
  }

  FTL::CStrRef getActualNewName()
    { return m_actualNewName; }

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    m_actualNewName =
      DFGUIPerform_RenameExecPort(
        binding,
        execPath,
        exec,
        m_oldName,
        m_desiredNewName,
        coreUndoCount
        );
  }

private:

  std::string m_oldName;
  std::string m_desiredNewName;

  std::string m_actualNewName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RenameExecPort_QUndo__
