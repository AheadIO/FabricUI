// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetNodeComment_QUndo__
#define __UI_DFG_DFGUICmd_SetNodeComment_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetNodeComment_QUndo
  : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_SetNodeComment_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef nodeName,
    FTL::StrRef comment
    )
    : DFGUICmd_Exec_QUndo(
      desc,
      binding,
      execPath,
      exec
      )
    , m_nodeName( nodeName )
    , m_comment( comment )
    {}

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    DFGUIPerform_SetNodeComment(
      binding,
      execPath,
      exec,
      m_nodeName,
      m_comment,
      coreUndoCount
      );
  }

private:

  std::string m_nodeName;
  std::string m_comment;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetNodeComment_QUndo__
