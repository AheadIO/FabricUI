// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_MoveNodes_QUndo__
#define __UI_DFG_DFGUICmd_MoveNodes_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_MoveNodes_QUndo
  : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_MoveNodes_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::ArrayRef<FTL::StrRef> nodeNames,
    FTL::ArrayRef<QPointF> newTopLeftPoss
    )
    : DFGUICmd_Exec_QUndo( desc, binding, execPath, exec )
  {
    m_nodeNames.insert(
      m_nodeNames.begin(),
      nodeNames.begin(),
      nodeNames.end()
      );
    m_newTopLeftPoss.insert(
      m_newTopLeftPoss.begin(),
      newTopLeftPoss.begin(),
      newTopLeftPoss.end()
      );
  }

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    DFGUIPerform_MoveNodes(
      binding,
      execPath,
      exec,
      m_nodeNames,
      m_newTopLeftPoss,
      coreUndoCount
      );
  }

private:

  std::vector<std::string> m_nodeNames;
  std::vector<QPointF> m_newTopLeftPoss;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_MoveNodes_QUndo__
