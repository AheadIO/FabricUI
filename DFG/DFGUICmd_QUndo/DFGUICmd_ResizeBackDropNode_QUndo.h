// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_ResizeBackDropNode_QUndo__
#define __UI_DFG_DFGUICmd_ResizeBackDropNode_QUndo__

#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_Exec_QUndo.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_ResizeBackDropNode_QUndo
  : public DFGUICmd_Exec_QUndo
{
public:

  DFGUICmd_ResizeBackDropNode_QUndo(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::StrRef backDropNodeName,
    QPointF newTopLeftPos,
    QSizeF newSize
    )
    : DFGUICmd_Exec_QUndo( desc, binding, execPath, exec )
    , m_backDropNodeName( backDropNodeName )
    , m_newTopLeftPos( newTopLeftPos )
    , m_newSize( newSize )
  {
  }

protected:
  
  virtual void invokeForExec(
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    unsigned &coreUndoCount
    )
  {
    DFGUIPerform_ResizeBackDropNode(
      binding,
      execPath,
      exec,
      m_backDropNodeName,
      m_newTopLeftPos,
      m_newSize,
      coreUndoCount
      );
  }

private:

  std::string m_backDropNodeName;
  QPointF m_newTopLeftPos;
  QSizeF m_newSize;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ResizeBackDropNode_QUndo__
