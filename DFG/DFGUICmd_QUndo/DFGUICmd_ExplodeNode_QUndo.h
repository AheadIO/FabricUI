// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_ExplodeNode_QUndo__
#define __UI_DFG_DFGUICmd_ExplodeNode_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_ExplodeNode_QUndo
  : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_ExplodeNode_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef nodeName
    )
    : DFGUICmd_Exec_QUndo(
      desc,
      binding,
      execPath,
      exec
      )
    , m_nodeName( nodeName )
    {}

  std::vector<std::string> const &getExplodedNodeNames()
    { return m_explodedNodeNames; }

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    m_explodedNodeNames =
      DFGUIPerform_ExplodeNode(
        binding,
        execPath,
        exec,
        m_nodeName,
        coreUndoCount
        );
  }

private:

  std::string m_nodeName;

  std::vector<std::string> m_explodedNodeNames;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ExplodeNode_QUndo__
