/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/DFG/DFGUICmdHandler_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddBackDrop_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddConnection_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddGet_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddInstFromPreset_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddInstWithEmptyFunc_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddInstWithEmptyGraph_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddPort_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddSet_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_AddVar_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_ExplodeNode_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_ImplodeNodes_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_MoveNodes_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_RemoveConnection_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_RemoveNodes_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_RemovePort_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_ResizeBackDropNode_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_SetNodeComment_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_SetNodeCommentExpanded_QUndo.h>
#include <FabricUI/DFG/DFGUICmd_QUndo/DFGUICmd_SetNodeTitle_QUndo.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

std::string DFGUICmdHandler_QUndo::dfgDoAddInstFromPreset(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef presetPath,
  QPointF pos
  )
{
  DFGUICmd_AddNode_QUndo *addNodeCmd =
    new DFGUICmd_AddInstFromPreset_QUndo(
      desc,
      binding,
      execPath,
      exec,
      presetPath,
      pos
      );
  m_qUndoStack->push( addNodeCmd );
  return addNodeCmd->getNodeName();
}

std::string DFGUICmdHandler_QUndo::dfgDoAddVar(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desireNodedName,
  FTL::StrRef dataType,
  FTL::StrRef extension,
  QPointF pos
  )
{
  DFGUICmd_AddNode_QUndo *addNodeCmd =
    new DFGUICmd_AddVar_QUndo(
      desc,
      binding,
      execPath,
      exec,
      desireNodedName,
      dataType,
      extension,
      pos
      );
  m_qUndoStack->push( addNodeCmd );
  return addNodeCmd->getNodeName();
}

std::string DFGUICmdHandler_QUndo::dfgDoAddGet(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desireNodedName,
  FTL::StrRef varPath,
  QPointF pos
  )
{
  DFGUICmd_AddNode_QUndo *addNodeCmd =
    new DFGUICmd_AddGet_QUndo(
      desc,
      binding,
      execPath,
      exec,
      desireNodedName,
      varPath,
      pos
      );
  m_qUndoStack->push( addNodeCmd );
  return addNodeCmd->getNodeName();
}

std::string DFGUICmdHandler_QUndo::dfgDoAddSet(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desireNodedName,
  FTL::StrRef varPath,
  QPointF pos
  )
{
  DFGUICmd_AddNode_QUndo *addNodeCmd =
    new DFGUICmd_AddSet_QUndo(
      desc,
      binding,
      execPath,
      exec,
      desireNodedName,
      varPath,
      pos
      );
  m_qUndoStack->push( addNodeCmd );
  return addNodeCmd->getNodeName();
}

std::string DFGUICmdHandler_QUndo::dfgDoAddInstWithEmptyGraph(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef title,
  QPointF pos
  )
{
  DFGUICmd_AddNode_QUndo *addNodeCmd =
    new DFGUICmd_AddInstWithEmptyGraph_QUndo(
      desc,
      binding,
      execPath,
      exec,
      title,
      pos
      );
  m_qUndoStack->push( addNodeCmd );
  return addNodeCmd->getNodeName();
}

std::string DFGUICmdHandler_QUndo::dfgDoAddInstWithEmptyFunc(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef title,
  FTL::CStrRef initialCode,
  QPointF pos
  )
{
  DFGUICmd_AddNode_QUndo *addNodeCmd =
    new DFGUICmd_AddInstWithEmptyFunc_QUndo(
      desc,
      binding,
      execPath,
      exec,
      title,
      initialCode,
      pos
      );
  m_qUndoStack->push( addNodeCmd );
  return addNodeCmd->getNodeName();
}

void DFGUICmdHandler_QUndo::dfgDoRemoveNodes(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::ArrayRef<FTL::StrRef> nodeNames
  )
{
  m_qUndoStack->push(
    new DFGUICmd_RemoveNodes_QUndo(
      desc,
      binding,
      execPath,
      exec,
      nodeNames
      )
    );
}

void DFGUICmdHandler_QUndo::dfgDoConnect(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::StrRef srcPath, 
  FTL::StrRef dstPath
  )
{
  m_qUndoStack->push(
    new DFGUICmd_AddConnection_QUndo(
      desc,
      binding,
      execPath,
      exec,
      srcPath,
      dstPath
      )
    );
}

void DFGUICmdHandler_QUndo::dfgDoDisconnect(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::StrRef srcPath, 
  FTL::StrRef dstPath
  )
{
  m_qUndoStack->push(
    new DFGUICmd_RemoveConnection_QUndo(
      desc,
      binding,
      execPath,
      exec,
      srcPath,
      dstPath
      )
    );
}

std::string DFGUICmdHandler_QUndo::dfgDoAddPort(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desiredPortName,
  FabricCore::DFGPortType dfgPortType,
  FTL::CStrRef typeSpec,
  FTL::CStrRef portToConnect
  )
{
  DFGUICmd_AddPort_QUndo *cmd =
    new DFGUICmd_AddPort_QUndo(
      desc,
      binding,
      execPath,
      exec,
      desiredPortName,
      dfgPortType,
      typeSpec,
      portToConnect
      );
  m_qUndoStack->push( cmd );
  return cmd->getPortName();
}

void DFGUICmdHandler_QUndo::dfgDoRemovePort(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef portName
  )
{
  DFGUICmd_RemovePort_QUndo *cmd =
    new DFGUICmd_RemovePort_QUndo(
      desc,
      binding,
      execPath,
      exec,
      portName
      );
  m_qUndoStack->push( cmd );
}

void DFGUICmdHandler_QUndo::dfgDoMoveNodes(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::ArrayRef<FTL::CStrRef> nodeNames,
  FTL::ArrayRef<QPointF> newTopLeftPoss
  )
{
  DFGUICmd_MoveNodes_QUndo *cmd =
    new DFGUICmd_MoveNodes_QUndo(
      desc,
      binding,
      execPath,
      exec,
      nodeNames,
      newTopLeftPoss
      );
  m_qUndoStack->push( cmd );
}

void DFGUICmdHandler_QUndo::dfgDoResizeBackDropNode(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef backDropNodeName,
  QPointF newTopLeftPos,
  QSizeF newSize
  )
{
  DFGUICmd_ResizeBackDropNode_QUndo *cmd =
    new DFGUICmd_ResizeBackDropNode_QUndo(
      desc,
      binding,
      execPath,
      exec,
      backDropNodeName,
      newTopLeftPos,
      newSize
      );
  m_qUndoStack->push( cmd );
}

std::string DFGUICmdHandler_QUndo::dfgDoImplodeNodes(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desiredNodeName,
  FTL::ArrayRef<FTL::CStrRef> nodeNames
  )
{
  DFGUICmd_ImplodeNodes_QUndo *cmd =
    new DFGUICmd_ImplodeNodes_QUndo(
      desc,
      binding,
      execPath,
      exec,
      desiredNodeName,
      nodeNames
      );
  m_qUndoStack->push( cmd );
  return cmd->getImplodedNodeName();
}

std::vector<std::string> DFGUICmdHandler_QUndo::dfgDoExplodeNode(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName
  )
{
  DFGUICmd_ExplodeNode_QUndo *cmd =
    new DFGUICmd_ExplodeNode_QUndo(
      desc,
      binding,
      execPath,
      exec,
      nodeName
      );
  m_qUndoStack->push( cmd );
  return cmd->getExplodedNodeNames();
}

void DFGUICmdHandler_QUndo::dfgDoAddBackDrop(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef title,
  QPointF pos
  )
{
  DFGUICmd_AddBackDrop_QUndo *cmd =
    new DFGUICmd_AddBackDrop_QUndo(
      desc,
      binding,
      execPath,
      exec,
      title,
      pos
      );
  m_qUndoStack->push( cmd );
}

void DFGUICmdHandler_QUndo::dfgDoSetNodeTitle(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  FTL::CStrRef newTitle
  )
{
  DFGUICmd_SetNodeTitle_QUndo *cmd =
    new DFGUICmd_SetNodeTitle_QUndo(
      desc,
      binding,
      execPath,
      exec,
      nodeName,
      newTitle
      );
  m_qUndoStack->push( cmd );
}

void DFGUICmdHandler_QUndo::dfgDoSetNodeComment(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  FTL::CStrRef comment
  )
{
  DFGUICmd_SetNodeComment_QUndo *cmd =
    new DFGUICmd_SetNodeComment_QUndo(
      desc,
      binding,
      execPath,
      exec,
      nodeName,
      comment
      );
  m_qUndoStack->push( cmd );
}

void DFGUICmdHandler_QUndo::dfgDoSetNodeCommentExpanded(
  FTL::CStrRef desc,
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  bool expanded
  )
{
  DFGUICmd_SetNodeCommentExpanded_QUndo *cmd =
    new DFGUICmd_SetNodeCommentExpanded_QUndo(
      desc,
      binding,
      execPath,
      exec,
      nodeName,
      expanded
      );
  m_qUndoStack->push( cmd );
}

FABRIC_UI_DFG_NAMESPACE_END
