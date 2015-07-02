// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_SetNodeexpandedExpanded_QUndo__
#define __UI_DFG_DFGUICmd_SetNodeexpandedExpanded_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_SetNodeCommentExpanded_QUndo
  : public DFGUICmd_QUndo
{
public:

  DFGUICmd_SetNodeCommentExpanded_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef nodeName,
    bool expanded
    )
    : DFGUICmd_QUndo(
      desc,
      binding,
      execPath,
      exec
      )
    , m_nodeName( nodeName )
    , m_expanded( expanded )
    {}

protected:
  
  virtual void invoke(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    DFGUIPerform_SetNodeCommentExpanded(
      binding,
      execPath,
      exec,
      m_nodeName,
      m_expanded,
      coreUndoCount
      );
  }

private:

  std::string m_nodeName;
  bool m_expanded;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_SetNodeexpandedExpanded_QUndo__
