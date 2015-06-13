// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmdHandler_QUndo__
#define __UI_DFG_DFGUICmdHandler_QUndo__

#include <FabricUI/DFG/DFGUICmdHandler.h>

#include <QtGui/QUndoStack>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmdHandler_QUndo : public DFGUICmdHandler
{
public:

  DFGUICmdHandler_QUndo(
    QUndoStack *qUndoStack
    )
    : m_qUndoStack( qUndoStack )
    {}

  virtual void dfgDoRemoveNodes(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::ArrayRef<FTL::StrRef> nodeNames
    );

  virtual void dfgDoConnect(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::StrRef srcPath, 
    FTL::StrRef dstPath
    );

  virtual void dfgDoDisconnect(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::StrRef srcPath, 
    FTL::StrRef dstPath
    );

  virtual std::string dfgDoAddInstWithEmptyGraph(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef title,
    QPointF pos
    );

  virtual std::string dfgDoAddInstWithEmptyFunc(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef title,
    FTL::CStrRef initialCode,
    QPointF pos
    );

  virtual std::string dfgDoAddInstFromPreset(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef presetPath,
    QPointF pos
    );

  virtual std::string dfgDoAddVar(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredNodeName,
    FTL::StrRef dataType,
    FTL::StrRef extDep,
    QPointF pos
    );

  virtual std::string dfgDoAddGet(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredNodeName,
    FTL::StrRef varPath,
    QPointF pos
    );

  virtual std::string dfgDoAddSet(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredNodeName,
    FTL::StrRef varPath,
    QPointF pos
    );

  virtual std::string dfgDoAddPort(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredPortName,
    FabricCore::DFGPortType portType,
    FTL::CStrRef typeSpec,
    FTL::CStrRef portToConnect
    );

  virtual void dfgDoRemovePort(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef portName
    );

  virtual std::string dfgDoImplodeNodes(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredNodeName,
    FTL::ArrayRef<FTL::CStrRef> nodeNames
    );

  virtual std::vector<std::string> dfgDoExplodeNode(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef nodeName
    );

private:

  QUndoStack *m_qUndoStack;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmdHandler_QUndo__
