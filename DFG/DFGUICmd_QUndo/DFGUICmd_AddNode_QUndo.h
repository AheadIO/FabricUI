// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddNode_QUndo__
#define __UI_DFG_DFGUICmd_AddNode_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>

#include <FTL/JSONValue.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddNode_QUndo : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_AddNode_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef primaryArg,
    QPointF pos
    )
    : DFGUICmd_Exec_QUndo( desc, binding, execPath, exec )
    , m_primaryArg( primaryArg )
    , m_pos( pos )
  {
  }

  FTL::CStrRef getNodeName() const
    { return m_nodeName; }

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    m_nodeName = invokeAdd(
      binding,
      execPath,
      exec,
      m_primaryArg,
      coreUndoCount
      );

    FTL::OwnedPtr<FTL::JSONObject> uiGraphPosVal( new FTL::JSONObject );
    uiGraphPosVal->insert(
      FTL_STR("x"),
      new FTL::JSONFloat64( m_pos.x() )
      );
    uiGraphPosVal->insert(
      FTL_STR("y"),
      new FTL::JSONFloat64( m_pos.y() )
      );

    exec.setNodeMetadata(
      m_nodeName.c_str(),
      "uiGraphPos",
      uiGraphPosVal->encode().c_str(),
      true // canUndo
      );
    ++coreUndoCount;
  }

  virtual FTL::CStrRef invokeAdd(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef primaryArg,
    unsigned &coreUndoCount
    ) = 0;

private:

  FTL::CStrRef m_primaryArg;
  QPointF m_pos;
  
  FTL::CStrRef m_nodeName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddNode_QUndo__
