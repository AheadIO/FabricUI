//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGUICmdHandler__
#define __UI_DFG_DFGUICmdHandler__

#include <FabricCore.h>

#include <FabricUI/DFG/DFGUINamespace.h>

#include <FTL/ArrayRef.h>
#include <FTL/CStrRef.h>

#include <QtCore/QPoint>
#include <QtCore/QSize>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmdHandler
{
public:

  virtual void dfgDoRemoveNodes(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::ArrayRef<FTL::CStrRef> nodeNames
    ) = 0;

  virtual void dfgDoConnect(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef srcPath, 
    FTL::CStrRef dstPath
    ) = 0;

  virtual void dfgDoDisconnect(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef srcPath, 
    FTL::CStrRef dstPath
    ) = 0;

  virtual std::string dfgDoAddGraph(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef title,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddFunc(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef title,
    FTL::CStrRef initialCode,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoInstPreset(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef presetPath,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddVar(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredNodeName,
    FTL::CStrRef dataType,
    FTL::CStrRef extDep,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddGet(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredNodeName,
    FTL::CStrRef varPath,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddSet(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredNodeName,
    FTL::CStrRef varPath,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddPort(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredPortName,
    FabricCore::DFGPortType portType,
    FTL::CStrRef typeSpec,
    FTL::CStrRef portToConnect
    ) = 0;

  virtual void dfgDoRemovePort(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef portName
    ) = 0;

  virtual void dfgDoMoveNodes(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::ArrayRef<FTL::CStrRef> nodeNames,
    FTL::ArrayRef<QPointF> newTopLeftPoss
    ) = 0;

  virtual void dfgDoResizeBackDrop(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef backDropNodeName,
    QPointF newTopLeftPos,
    QSizeF newSize
    ) = 0;

  virtual std::string dfgDoImplodeNodes(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredNodeName,
    FTL::ArrayRef<FTL::CStrRef> nodeNames
    ) = 0;

  virtual std::vector<std::string> dfgDoExplodeNode(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef nodeName
    ) = 0;

  virtual void dfgDoAddBackDrop(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef title,
    QPointF pos
    ) = 0;

  virtual void dfgDoSetNodeTitle(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef nodeName,
    FTL::CStrRef title
    ) = 0;

  virtual void dfgDoSetNodeComment(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef nodeName,
    FTL::CStrRef comment
    ) = 0;

  virtual void dfgDoSetCode(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef code
    ) = 0;

  virtual std::string dfgDoRenamePort(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef oldPortName,
    FTL::CStrRef desiredNewPortName
    ) = 0;

  virtual std::vector<std::string> dfgDoPaste(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef json,
    QPointF cursorPos
    ) = 0;

  virtual void dfgDoSetArgType(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef argName,
    FTL::CStrRef typeName
    ) = 0;

  virtual void dfgDoSetArgValue(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef argName,
    FabricCore::RTVal const &value
    ) = 0;

  virtual void dfgDoSetPortDefaultValue(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef portPath,
    FabricCore::RTVal const &value
    ) = 0;

  virtual void dfgDoSetRefVarPath(
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef refName,
    FTL::CStrRef varPath
    ) = 0;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmdHandler__
