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
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::ArrayRef<FTL::StrRef> nodeNames
    ) = 0;

  virtual void dfgDoConnect(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::StrRef srcPath, 
    FTL::StrRef dstPath
    ) = 0;

  virtual void dfgDoDisconnect(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::StrRef srcPath, 
    FTL::StrRef dstPath
    ) = 0;

  virtual std::string dfgDoAddInstWithEmptyGraph(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef title,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddInstWithEmptyFunc(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef title,
    FTL::CStrRef initialCode,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddInstFromPreset(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef presetPath,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddVar(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredNodeName,
    FTL::StrRef dataType,
    FTL::StrRef extDep,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddGet(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredNodeName,
    FTL::StrRef varPath,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddSet(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredNodeName,
    FTL::StrRef varPath,
    QPointF pos
    ) = 0;

  virtual std::string dfgDoAddPort(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredPortName,
    FabricCore::DFGPortType portType,
    FTL::CStrRef typeSpec,
    FTL::CStrRef portToConnect
    ) = 0;

  virtual void dfgDoRemovePort(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef portName
    ) = 0;

  virtual void dfgDoMoveNodes(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::ArrayRef<FTL::CStrRef> nodeNames,
    FTL::ArrayRef<QPointF> newTopLeftPoss
    ) = 0;

  virtual void dfgDoResizeBackDropNode(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef backDropNodeName,
    QPointF newTopLeftPos,
    QSizeF newSize
    ) = 0;

  virtual std::string dfgDoImplodeNodes(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef desiredNodeName,
    FTL::ArrayRef<FTL::CStrRef> nodeNames
    ) = 0;

  virtual std::vector<std::string> dfgDoExplodeNode(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef nodeName
    ) = 0;

  virtual void dfgDoAddBackDrop(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef title,
    QPointF pos
    ) = 0;

  virtual void dfgDoSetNodeTitle(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef nodeName,
    FTL::CStrRef newTitle
    ) = 0;

  virtual void dfgDoSetNodeComment(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef nodeName,
    FTL::CStrRef comment
    ) = 0;

  virtual void dfgDoSetNodeCommentExpanded(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef nodeName,
    bool expanded
    ) = 0;

  virtual void dfgDoSetCode(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef code
    ) = 0;

  virtual std::string dfgDoRenameExecPort(
    FTL::CStrRef desc,
    FabricCore::DFGBinding &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec &exec,
    FTL::CStrRef oldName,
    FTL::CStrRef desiredNewName
    ) = 0;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmdHandler__
