/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/DFG/DFGUICmd/DFGUICmds.h>
#include <FabricUI/DFG/DFGUICmdHandler_QUndo.h>

#include <FTL/OwnedPtr.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmdHandler_QUndo::WrappedCommand : public QUndoCommand
{
public:

  WrappedCommand( DFGUICmd *dfgUICmd )
    : QUndoCommand()
    , m_dfgUICmd( dfgUICmd )
    , m_didit( false )
  {
  }

protected:

  virtual void redo()
  {
    try
    {
      if ( m_didit )
      {
        m_dfgUICmd->redo();
      }
      else
      {
        m_didit = true;
        m_dfgUICmd->doit();
        QUndoCommand::setText( m_dfgUICmd->getDesc() );
      }
    }
    catch ( FabricCore::Exception e )
    {
      printf(
        "Caught FabricCore::Exception: %s\n",
        e.getDesc_cstr()
        );
    }
  }

  virtual void undo()
  {
    try
    {
      assert( m_didit );
      m_dfgUICmd->undo();
    }
    catch ( FabricCore::Exception e )
    {
      printf(
        "Caught FabricCore::Exception: %s\n",
        e.getDesc_cstr()
        );
    }
  }

private:

  FTL::OwnedPtr<DFGUICmd> m_dfgUICmd;
  bool m_didit;
};

QString DFGUICmdHandler_QUndo::dfgDoInstPreset(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString presetPath,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_InstPreset(
      binding,
      execPath,
      exec,
      presetPath,
      pos
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualNodeName();
}

QString DFGUICmdHandler_QUndo::dfgDoAddVar(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString desireNodedName,
  QString dataType,
  QString extension,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_AddVar(
      binding,
      execPath,
      exec,
      desireNodedName,
      dataType,
      extension,
      pos
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualNodeName();
}

QString DFGUICmdHandler_QUndo::dfgDoAddGet(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString desireNodedName,
  QString varPath,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_AddGet(
      binding,
      execPath,
      exec,
      desireNodedName,
      varPath,
      pos
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualNodeName();
}

QString DFGUICmdHandler_QUndo::dfgDoAddSet(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString desireNodedName,
  QString varPath,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_AddSet(
      binding,
      execPath,
      exec,
      desireNodedName,
      varPath,
      pos
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualNodeName();
}

QString DFGUICmdHandler_QUndo::dfgDoAddGraph(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString title,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_AddGraph(
      binding,
      execPath,
      exec,
      title,
      pos
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualNodeName();
}

QString DFGUICmdHandler_QUndo::dfgDoAddFunc(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString title,
  QString initialCode,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_AddFunc(
      binding,
      execPath,
      exec,
      title,
      initialCode,
      pos
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualNodeName();
}

void DFGUICmdHandler_QUndo::dfgDoRemoveNodes(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QStringList nodeNames
  )
{
  m_qUndoStack->push(
    new WrappedCommand(
      new DFGUICmd_RemoveNodes(
        binding,
        execPath,
        exec,
        nodeNames
        )
      )
    );
}

void DFGUICmdHandler_QUndo::dfgDoConnect(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString srcPath, 
  QString dstPath
  )
{
  m_qUndoStack->push(
    new WrappedCommand(
      new DFGUICmd_Connect(
        binding,
        execPath,
        exec,
        srcPath,
        dstPath
        )
      )
    );
}

void DFGUICmdHandler_QUndo::dfgDoDisconnect(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString srcPath, 
  QString dstPath
  )
{
  m_qUndoStack->push(
    new WrappedCommand(
      new DFGUICmd_Disconnect(
        binding,
        execPath,
        exec,
        srcPath,
        dstPath
        )
      )
    );
}

QString DFGUICmdHandler_QUndo::dfgDoAddPort(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString desiredPortName,
  FabricCore::DFGPortType dfgPortType,
  QString typeSpec,
  QString portToConnect,
  QString extDep,
  QString metaData
  )
{
  DFGUICmd_AddPort *cmd =
    new DFGUICmd_AddPort(
      binding,
      execPath,
      exec,
      desiredPortName,
      dfgPortType,
      typeSpec,
      portToConnect,
      extDep,
      metaData
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualPortName();
}

QString DFGUICmdHandler_QUndo::dfgDoEditPort(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString oldPortName,
  QString desiredNewPortName,
  QString typeSpec,
  QString extDep,
  QString uiMetadata
  )
{
  DFGUICmd_EditPort *cmd =
    new DFGUICmd_EditPort(
      binding,
      execPath,
      exec,
      oldPortName,
      desiredNewPortName,
      typeSpec,
      extDep,
      uiMetadata
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualNewPortName();
}

QString DFGUICmdHandler_QUndo::dfgDoCreatePreset(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString nodeName,
  QString presetDirPath,
  QString presetName
  )
{
  DFGUICmd_CreatePreset *cmd =
    new DFGUICmd_CreatePreset(
      binding,
      execPath,
      exec,
      nodeName,
      presetDirPath,
      presetName
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getPathname();
}

void DFGUICmdHandler_QUndo::dfgDoRemovePort(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString portName
  )
{
  DFGUICmd_RemovePort *cmd =
    new DFGUICmd_RemovePort(
      binding,
      execPath,
      exec,
      portName
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoMoveNodes(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QStringList nodeNames,
  QList<QPointF> newTopLeftPoss
  )
{
  DFGUICmd_MoveNodes *cmd =
    new DFGUICmd_MoveNodes(
      binding,
      execPath,
      exec,
      nodeNames,
      newTopLeftPoss
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoResizeBackDrop(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString backDropNodeName,
  QPointF newTopLeftPos,
  QSizeF newSize
  )
{
  DFGUICmd_ResizeBackDrop *cmd =
    new DFGUICmd_ResizeBackDrop(
      binding,
      execPath,
      exec,
      backDropNodeName,
      newTopLeftPos,
      newSize
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

QString DFGUICmdHandler_QUndo::dfgDoImplodeNodes(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QStringList nodeNames,
  QString desiredImplodedNodeName
  )
{
  DFGUICmd_ImplodeNodes *cmd =
    new DFGUICmd_ImplodeNodes(
      binding,
      execPath,
      exec,
      nodeNames,
      desiredImplodedNodeName
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualImplodedNodeName();
}

QStringList DFGUICmdHandler_QUndo::dfgDoExplodeNode(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString nodeName
  )
{
  DFGUICmd_ExplodeNode *cmd =
    new DFGUICmd_ExplodeNode(
      binding,
      execPath,
      exec,
      nodeName
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );

  return cmd->getExplodedNodeNames();
}

void DFGUICmdHandler_QUndo::dfgDoAddBackDrop(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString title,
  QPointF pos
  )
{
  DFGUICmd_AddBackDrop *cmd =
    new DFGUICmd_AddBackDrop(
      binding,
      execPath,
      exec,
      title,
      pos
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoSetNodeComment(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString nodeName,
  QString comment
  )
{
  DFGUICmd_SetNodeComment *cmd =
    new DFGUICmd_SetNodeComment(
      binding,
      execPath,
      exec,
      nodeName,
      comment
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoSetCode(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString code
  )
{
  DFGUICmd_SetCode *cmd =
    new DFGUICmd_SetCode(
      binding,
      execPath,
      exec,
      code
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

QString DFGUICmdHandler_QUndo::dfgDoRenamePort(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString oldName,
  QString desiredNewName
  )
{
  DFGUICmd_RenamePort *cmd =
    new DFGUICmd_RenamePort(
      binding,
      execPath,
      exec,
      oldName,
      desiredNewName
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualNewPortName();
}

QString DFGUICmdHandler_QUndo::dfgDoEditNode(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString oldNodeName,
  QString desiredNewNodeName,
  QString nodeMetadata,
  QString execMetadata
  )
{
  DFGUICmd_EditNode *cmd =
    new DFGUICmd_EditNode(
      binding,
      execPath,
      exec,
      oldNodeName,
      desiredNewNodeName,
      nodeMetadata,
      execMetadata
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualNewNodeName();
}

QStringList DFGUICmdHandler_QUndo::dfgDoPaste(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString json,
  QPointF cursorPos
  )
{
  DFGUICmd_Paste *cmd =
    new DFGUICmd_Paste(
      binding,
      execPath,
      exec,
      json,
      cursorPos
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getPastedNodeNames();
}

void DFGUICmdHandler_QUndo::dfgDoSetArgValue(
  FabricCore::DFGBinding const &binding,
  QString argName,
  FabricCore::RTVal const &value
  )
{
  DFGUICmd_SetArgValue *cmd =
    new DFGUICmd_SetArgValue(
      binding,
      argName,
      value
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoSetPortDefaultValue(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString portOrPinPath,
  FabricCore::RTVal const &value
  )
{
  DFGUICmd_SetPortDefaultValue *cmd =
    new DFGUICmd_SetPortDefaultValue(
      binding,
      execPath,
      exec,
      portOrPinPath,
      value
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoSetRefVarPath(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString refName,
  QString varPath
  )
{
  DFGUICmd_SetRefVarPath *cmd =
    new DFGUICmd_SetRefVarPath(
      binding,
      execPath,
      exec,
      refName,
      varPath
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoReorderPorts(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QList<int> indices
  )
{
  DFGUICmd_ReorderPorts *cmd =
    new DFGUICmd_ReorderPorts(
      binding,
      execPath,
      exec,
      indices
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoSetExtDeps(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QStringList extDeps
  )
{
  DFGUICmd_SetExtDeps *cmd =
    new DFGUICmd_SetExtDeps(
      binding,
      execPath,
      exec,
      extDeps
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoSplitFromPreset(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec
  )
{
  DFGUICmd_SplitFromPreset *cmd =
    new DFGUICmd_SplitFromPreset(
      binding,
      execPath,
      exec
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

FABRIC_UI_DFG_NAMESPACE_END
