// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetRefVarPath_QUndo__
#define __UI_DFG_DFGUICmd_SetRefVarPath_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetRefVarPath_QUndo
  : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_SetRefVarPath_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef refName,
    FTL::StrRef varPath
    )
    : DFGUICmd_Exec_QUndo(
      desc,
      binding,
      execPath,
      exec
      )
    , m_refName( refName )
    , m_varPath( varPath )
    {}

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    DFGUIPerform_SetRefVarPath(
      binding,
      execPath,
      exec,
      m_refName,
      m_varPath,
      coreUndoCount
      );
  }

private:

  std::string m_refName;
  std::string m_varPath;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetRefVarPath_QUndo__
