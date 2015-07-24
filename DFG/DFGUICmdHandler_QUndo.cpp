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
        QUndoCommand::setText( m_dfgUICmd->getDesc().c_str() );
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

std::string DFGUICmdHandler_QUndo::dfgDoInstPreset(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef presetPath,
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

std::string DFGUICmdHandler_QUndo::dfgDoAddVar(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef desireNodedName,
  FTL::CStrRef dataType,
  FTL::CStrRef extension,
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

std::string DFGUICmdHandler_QUndo::dfgDoAddGet(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef desireNodedName,
  FTL::CStrRef varPath,
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

std::string DFGUICmdHandler_QUndo::dfgDoAddSet(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef desireNodedName,
  FTL::CStrRef varPath,
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

std::string DFGUICmdHandler_QUndo::dfgDoAddGraph(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef title,
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

std::string DFGUICmdHandler_QUndo::dfgDoAddFunc(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef title,
  FTL::CStrRef initialCode,
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
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::ArrayRef<FTL::CStrRef> nodeNames
  )
{
  m_qUndoStack->push(
    new WrappedCommand(
      new DFGUICmd_RemoveNodes(
        binding,
        execPath,
        exec,
        FTL::ArrayRef<FTL::StrRef>( nodeNames.begin(), nodeNames.end() )
        )
      )
    );
}

void DFGUICmdHandler_QUndo::dfgDoConnect(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef srcPath, 
  FTL::CStrRef dstPath
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
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef srcPath, 
  FTL::CStrRef dstPath
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

std::string DFGUICmdHandler_QUndo::dfgDoAddPort(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef desiredPortName,
  FabricCore::DFGPortType dfgPortType,
  FTL::CStrRef typeSpec,
  FTL::CStrRef portToConnect
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
      portToConnect
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualPortName();
}

void DFGUICmdHandler_QUndo::dfgDoRemovePort(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef portName
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
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::ArrayRef<FTL::CStrRef> nodeNames,
  FTL::ArrayRef<QPointF> newTopLeftPoss
  )
{
  DFGUICmd_MoveNodes *cmd =
    new DFGUICmd_MoveNodes(
      binding,
      execPath,
      exec,
      FTL::ArrayRef<FTL::StrRef>( nodeNames.begin(), nodeNames.end() ),
      newTopLeftPoss
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoResizeBackDrop(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef backDropNodeName,
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

std::string DFGUICmdHandler_QUndo::dfgDoImplodeNodes(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef desiredImplodedNodeName,
  FTL::ArrayRef<FTL::CStrRef> nodeNames
  )
{
  DFGUICmd_ImplodeNodes *cmd =
    new DFGUICmd_ImplodeNodes(
      binding,
      execPath,
      exec,
      desiredImplodedNodeName,
      FTL::ArrayRef<FTL::StrRef>( nodeNames.begin(), nodeNames.end() )
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
  return cmd->getActualImplodedNodeName();
}

std::vector<std::string> DFGUICmdHandler_QUndo::dfgDoExplodeNode(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef nodeName
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

  std::vector<std::string> explodedNodeNames;
  explodedNodeNames.insert(
    explodedNodeNames.end(),
    cmd->getExplodedNodeNames().begin(),
    cmd->getExplodedNodeNames().end()
    );
  return explodedNodeNames;
}

void DFGUICmdHandler_QUndo::dfgDoAddBackDrop(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef title,
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

void DFGUICmdHandler_QUndo::dfgDoSetNodeTitle(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef nodeName,
  FTL::CStrRef newTitle
  )
{
  DFGUICmd_SetNodeTitle *cmd =
    new DFGUICmd_SetNodeTitle(
      binding,
      execPath,
      exec,
      nodeName,
      newTitle
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoSetNodeComment(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef nodeName,
  FTL::CStrRef comment,
  bool expanded
  )
{
  DFGUICmd_SetNodeComment *cmd =
    new DFGUICmd_SetNodeComment(
      binding,
      execPath,
      exec,
      nodeName,
      comment,
      expanded
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoSetCode(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef code
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

std::string DFGUICmdHandler_QUndo::dfgDoRenamePort(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef oldName,
  FTL::CStrRef desiredNewName
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

std::vector<std::string> DFGUICmdHandler_QUndo::dfgDoPaste(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef json,
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

  std::vector<std::string> pastedNodeNames;
  pastedNodeNames.insert(
    pastedNodeNames.end(),
    cmd->getPastedNodeNames().begin(),
    cmd->getPastedNodeNames().end()
    );
  return pastedNodeNames;
}

void DFGUICmdHandler_QUndo::dfgDoSetArgType(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef argName,
  FTL::CStrRef typeName
  )
{
  DFGUICmd_SetArgType *cmd =
    new DFGUICmd_SetArgType(
      binding,
      argName,
      typeName
      );
  m_qUndoStack->push( new WrappedCommand( cmd ) );
}

void DFGUICmdHandler_QUndo::dfgDoSetArgValue(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef argName,
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
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef portOrPinPath,
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
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef refName,
  FTL::CStrRef varPath
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

FABRIC_UI_DFG_NAMESPACE_END
