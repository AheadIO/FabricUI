// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddBackDrop_QUndo__
#define __UI_DFG_DFGUICmd_AddBackDrop_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>
#include <FabricUI/DFG/DFGUIPerform.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddBackDrop_QUndo
  : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_AddBackDrop_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef title,
    QPointF pos
    )
    : DFGUICmd_Exec_QUndo(
      desc,
      binding,
      execPath,
      exec
      )
    , m_title( title )
    , m_pos( pos )
    {}

  FTL::CStrRef getNodeName()
    { return m_nodeName; }

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    m_nodeName = DFGUIPerform_AddBackDrop(
      binding,
      execPath,
      exec,
      m_title,
      m_pos,
      coreUndoCount
      );
  }

private:

  std::string m_title;
  QPointF m_pos;

  std::string m_nodeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddBackDrop_QUndo__
