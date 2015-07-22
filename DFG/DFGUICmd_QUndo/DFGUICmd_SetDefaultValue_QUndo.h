// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetDefaultValue_QUndo__
#define __UI_DFG_DFGUICmd_SetDefaultValue_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetDefaultValue_QUndo
  : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_SetDefaultValue_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef portOrPinPath,
    FabricCore::RTVal const &value
    )
    : DFGUICmd_Exec_QUndo(
      desc,
      binding,
      execPath,
      exec
      )
    , m_portOrPinPath( portOrPinPath )
    , m_value( value )
    {}

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    DFGUIPerform_SetDefaultValue(
      binding,
      execPath,
      exec,
      m_portOrPinPath,
      m_value,
      coreUndoCount
      );
  }

private:

  std::string m_portOrPinPath;
  FabricCore::RTVal m_value;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetDefaultValue_QUndo__
