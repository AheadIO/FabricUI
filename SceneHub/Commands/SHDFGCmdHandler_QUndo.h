/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SCENEHUB_DFG_CMDHANDLER_QUNDO_H__
#define __FABRICUI_SCENEHUB_DFG_CMDHANDLER_QUNDO_H__

#include <QtGui/QUndoStack>
#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONValue.h>
#include <FTL/Str.h>
#include <FTL/JSONEnc.h>
#include <FTL/MapCharSingle.h>
#include <FabricUI/Util/macros.h>
#include <FabricUI/SceneHub/Commands/SHCmd.h>
#include <FabricUI/Canvas/DFG/DFGUICmdHandler.h>
#include <FabricUI/Canvas/DFG/DFGUICmdHandler_QUndo.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHDFGCmdHandler_QUndo : public FabricUI::DFG::DFGUICmdHandler_QUndo
    {
      protected:
        class WrappedCmd;

      public:
        /// Default constructor
        SHDFGCmdHandler_QUndo() : FabricUI::DFG::DFGUICmdHandler_QUndo() {};

        /// Constructs a new SHDFGCmdHandler_QUndo.
        /// \param qUndoStack A pointer to the Qt undo-redo stack
        SHDFGCmdHandler_QUndo(QUndoStack *qUndoStack) : FabricUI::DFG::DFGUICmdHandler_QUndo(qUndoStack) {
          std::cerr << "SHDFGCmdHandler_QUndo constructor" << std::endl;
        };
  
        virtual ~SHDFGCmdHandler_QUndo() {};

        // ***** Canvas *****
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoRemoveNodes(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::ArrayRef<FTL::StrRef> nodeNames
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoConnect(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef srcPath, 
          FTL::CStrRef dstPath
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoDisconnect(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef srcPath, 
          FTL::CStrRef dstPath
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoAddGraph(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef title,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoAddFunc(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef title,
          FTL::CStrRef initialCode,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoInstPreset(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef presetPath,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoAddVar(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef desiredNodeName,
          FTL::CStrRef dataType,
          FTL::CStrRef extDep,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoAddGet(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef desiredNodeName,
          FTL::CStrRef varPath,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoAddSet(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef desiredNodeName,
          FTL::CStrRef varPath,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoAddPort(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef desiredPortName,
          FabricCore::DFGPortType portType,
          FTL::CStrRef typeSpec,
          FTL::CStrRef portToConnect,
          FTL::StrRef extDep,
          FTL::CStrRef metaData
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoCreatePreset(
          FabricCore::DFGBinding const &binding,
          FTL::StrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::StrRef nodeName,
          FTL::StrRef presetDirPath,
          FTL::StrRef presetName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoEditPort(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::StrRef oldPortName,
          FTL::StrRef desiredNewPortName,
          FTL::StrRef typeSpec,
          FTL::StrRef extDep,
          FTL::StrRef uiMetadata
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoRemovePort(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef portName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoResizeBackDrop(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef backDropNodeName,
          QPointF newTopLeftPos,
          QSizeF newSize
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo        
        virtual void dfgDoMoveNodes(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::ArrayRef<FTL::StrRef> nodeNames,
          FTL::ArrayRef<QPointF> newTopLeftPoss
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoImplodeNodes(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::ArrayRef<FTL::StrRef> nodeNames,
          FTL::CStrRef desiredNodeName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::vector<std::string> dfgDoExplodeNode(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef nodeName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoAddBackDrop(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef title,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetTitle(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef newTitle
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetNodeComment(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef nodeName,
          FTL::CStrRef comment
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetCode(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef code
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoEditNode(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::StrRef oldNodeName,
          FTL::StrRef desiredNewNodeName,
          FTL::StrRef nodeMetadata,
          FTL::StrRef execMetadata
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::string dfgDoRenamePort(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef oldName,
          FTL::CStrRef desiredNewName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual std::vector<std::string> dfgDoPaste(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef json,
          QPointF cursorPos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetArgType(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef argName,
          FTL::CStrRef typeName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetArgValue(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef argName,
          FabricCore::RTVal const &value
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetPortDefaultValue(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef portOrPinPath,
          FabricCore::RTVal const &value
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetRefVarPath(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::CStrRef refName,
          FTL::CStrRef varPath
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoReorderPorts(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          const std::vector<unsigned int> & indices
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetExtDeps(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec,
          FTL::ArrayRef<FTL::StrRef> extDeps
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSplitFromPreset(
          FabricCore::DFGBinding const &binding,
          FTL::CStrRef execPath,
          FabricCore::DFGExec const &exec
          );

        /// Gets a pointer to the qt command stack.
        QUndoStack* getStack() { return m_qUndoStack; };
        
        /// Adds and executes a command
        /// \param cmd The command
        bool addCommand(SHCmd *cmd);
    };
  };
};

#endif // __FABRICUI_SCENEHUB_DFG_CMDHANDLER_QUNDO_H__

