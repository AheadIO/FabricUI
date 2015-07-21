// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetNodeTitle_QUndo__
#define __UI_DFG_DFGUICmd_SetNodeTitle_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetNodeTitle_QUndo
  : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_SetNodeTitle_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef nodeName,
    FTL::StrRef newTitle
    )
    : DFGUICmd_Exec_QUndo(
      desc,
      binding,
      execPath,
      exec
      )
    , m_nodeName( nodeName )
    , m_newTitle( newTitle )
    {}

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    DFGUIPerform_SetNodeTitle(
      binding,
      execPath,
      exec,
      m_nodeName,
      m_newTitle,
      coreUndoCount
      );
  }

private:

  std::string m_nodeName;
  std::string m_newTitle;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetNodeTitle_QUndo__
