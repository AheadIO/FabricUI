/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/Canvas/DFG/DFGUICmd/DFGUICmds.h>
#include <FabricUI/SceneHub/Commands/SHCmdHandler_QUndo.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace FabricUI::DFG;
using namespace FabricUI::SceneHub;


/// \internal
/// Wrap a SHCmd into a Qt command
class SHCmdHandler_QUndo::WrappedCmd : public QUndoCommand {

  public:
    WrappedCmd( SHCmd *shCmd ) : QUndoCommand(), 
      m_shCmd( shCmd ),
      m_didit( false ) 
      {
        m_dfgUICmd = 0;
      };

    WrappedCmd( FabricUI::DFG::DFGUICmd *dfgUICmd ) : QUndoCommand(), 
      m_dfgUICmd( dfgUICmd ),
      m_didit( false )
      {
        m_shCmd = 0;
      };


  protected:
    virtual void redo() 
    {
      try 
      {
        if(!m_dfgUICmd && m_shCmd)
        {
          if(m_didit) 
          {
            m_shCmd->redo();
          }
          else 
          {
            m_didit = true;
            m_shCmd->doit();
          }
          QUndoCommand::setText( m_shCmd->getDesc().c_str() );
        }
        else if(m_dfgUICmd && !m_shCmd) 
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
      }
      catch (FabricCore::Exception e) {
        printf("Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
      }
    }

    virtual void undo() {
      try 
      {
        assert( m_didit );
        if(!m_dfgUICmd && m_shCmd)
        {
          m_shCmd->undo();
        }
        else if(m_dfgUICmd && !m_shCmd)
        {
          m_dfgUICmd->undo();
        }
      }
      catch (FabricCore::Exception e) {
        printf( "Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
      }
    }

  private:
    FTL::OwnedPtr<SHCmd> m_shCmd;
    FTL::OwnedPtr<FabricUI::DFG::DFGUICmd> m_dfgUICmd;
    bool m_didit;
};

// ***** DFG Commands *****
std::string SHCmdHandler_QUndo::dfgDoInstPreset(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

std::string SHCmdHandler_QUndo::dfgDoAddVar(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

std::string SHCmdHandler_QUndo::dfgDoAddGet(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

std::string SHCmdHandler_QUndo::dfgDoAddSet(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

std::string SHCmdHandler_QUndo::dfgDoAddGraph(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

std::string SHCmdHandler_QUndo::dfgDoAddFunc(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

void SHCmdHandler_QUndo::dfgDoRemoveNodes(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::ArrayRef<FTL::StrRef> nodeNames
  )
{
  m_qUndoStack->push(
    new WrappedCmd(
      new DFGUICmd_RemoveNodes(
        binding,
        execPath,
        exec,
        FTL::ArrayRef<FTL::StrRef>( nodeNames.begin(), nodeNames.end() )
        )
      )
    );
}

void SHCmdHandler_QUndo::dfgDoConnect(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef srcPath, 
  FTL::CStrRef dstPath
  )
{
  m_qUndoStack->push(
    new WrappedCmd(
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

void SHCmdHandler_QUndo::dfgDoDisconnect(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef srcPath, 
  FTL::CStrRef dstPath
  )
{
  m_qUndoStack->push(
    new WrappedCmd(
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

std::string SHCmdHandler_QUndo::dfgDoAddPort(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef desiredPortName,
  FabricCore::DFGPortType dfgPortType,
  FTL::CStrRef typeSpec,
  FTL::CStrRef portToConnect,
  FTL::StrRef extDep,
  FTL::CStrRef metaData
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualPortName();
}

std::string SHCmdHandler_QUndo::dfgDoEditPort(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::StrRef oldPortName,
  FTL::StrRef desiredNewPortName,
  FTL::StrRef typeSpec,
  FTL::StrRef extDep,
  FTL::StrRef uiMetadata
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNewPortName();
}

std::string SHCmdHandler_QUndo::dfgDoCreatePreset(
  FabricCore::DFGBinding const &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::StrRef nodeName,
  FTL::StrRef presetDirPath,
  FTL::StrRef presetName
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getPathname();
}

void SHCmdHandler_QUndo::dfgDoRemovePort(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoMoveNodes(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::ArrayRef<FTL::StrRef> nodeNames,
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoResizeBackDrop(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

std::string SHCmdHandler_QUndo::dfgDoImplodeNodes(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::ArrayRef<FTL::StrRef> nodeNames,
  FTL::CStrRef desiredImplodedNodeName
  )
{
  DFGUICmd_ImplodeNodes *cmd =
    new DFGUICmd_ImplodeNodes(
      binding,
      execPath,
      exec,
      FTL::ArrayRef<FTL::StrRef>( nodeNames.begin(), nodeNames.end() ),
      desiredImplodedNodeName
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualImplodedNodeName();
}

std::vector<std::string> SHCmdHandler_QUndo::dfgDoExplodeNode(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );

  std::vector<std::string> explodedNodeNames;
  explodedNodeNames.insert(
    explodedNodeNames.end(),
    cmd->getExplodedNodeNames().begin(),
    cmd->getExplodedNodeNames().end()
    );
  return explodedNodeNames;
}

void SHCmdHandler_QUndo::dfgDoAddBackDrop(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoSetTitle(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef newTitle
  )
{
  DFGUICmd_SetTitle *cmd =
    new DFGUICmd_SetTitle(
      binding,
      execPath,
      exec,
      newTitle
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoSetNodeComment(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::CStrRef nodeName,
  FTL::CStrRef comment
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoSetCode(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

std::string SHCmdHandler_QUndo::dfgDoRenamePort(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNewPortName();
}

std::string SHCmdHandler_QUndo::dfgDoEditNode(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::StrRef oldNodeName,
  FTL::StrRef desiredNewNodeName,
  FTL::StrRef nodeMetadata,
  FTL::StrRef execMetadata
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNewNodeName();
}

std::vector<std::string> SHCmdHandler_QUndo::dfgDoPaste(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );

  std::vector<std::string> pastedNodeNames;
  pastedNodeNames.insert(
    pastedNodeNames.end(),
    cmd->getPastedNodeNames().begin(),
    cmd->getPastedNodeNames().end()
    );
  return pastedNodeNames;
}

void SHCmdHandler_QUndo::dfgDoSetArgType(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoSetArgValue(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoSetPortDefaultValue(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoSetRefVarPath(
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
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoReorderPorts(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  const std::vector<unsigned int> & indices
  )
{
  DFGUICmd_ReorderPorts *cmd =
    new DFGUICmd_ReorderPorts(
      binding,
      execPath,
      exec,
      indices
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoSetExtDeps(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::ArrayRef<FTL::StrRef> extDeps
  )
{
  DFGUICmd_SetExtDeps *cmd =
    new DFGUICmd_SetExtDeps(
      binding,
      execPath,
      exec,
      extDeps
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHCmdHandler_QUndo::dfgDoSplitFromPreset(
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec const &exec
  )
{
  DFGUICmd_SplitFromPreset *cmd =
    new DFGUICmd_SplitFromPreset(
      binding,
      execPath,
      exec
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}


// ***** SceneHub Commands *****
/// Adds an object to the scene-graph
/// \param command The command as string
bool SHCmdHandler_QUndo::addCommand(SHCmd *cmd) {
  if(cmd) {
    // Clears the kl undo stack --> synchronize Qt and K stacks
    SHCmd::GetCmdManager(cmd->getRefOnSCeneHub()).callMethod("", "clearRedoStack", 0, 0);
    m_qUndoStack->push( new WrappedCmd(cmd) );
    return true;
  }
  else return false;
}


