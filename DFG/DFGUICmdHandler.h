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

  static FTL::CStrRef CmdName_RemoveNodes()
    { return FTL_STR("dfgRemoveNodes"); }

  virtual ~DFGUICmdHandler() {}

  virtual void dfgDoRemoveNodes(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::ArrayRef<FTL::CStrRef> nodeNames
    ) = 0;

  static FTL::CStrRef CmdName_Connect()
    { return FTL_STR("dfgConnect"); }
  virtual void dfgDoConnect(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef srcPath, 
    FTL::CStrRef dstPath
    ) = 0;

  static FTL::CStrRef CmdName_Disconnect()
    { return FTL_STR("dfgDisconnect"); }
  virtual void dfgDoDisconnect(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef srcPath, 
    FTL::CStrRef dstPath
    ) = 0;

  static FTL::CStrRef CmdName_AddInstWithEmptyGraph()
    { return FTL_STR("dfgAddInstWithEmptyGraph"); }
  virtual std::string dfgDoAddInstWithEmptyGraph(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef title,
    QPointF pos
    ) = 0;

  static FTL::CStrRef CmdName_AddInstWithEmptyFunc()
    { return FTL_STR("dfgAddInstWithEmptyFunc"); }
  virtual std::string dfgDoAddInstWithEmptyFunc(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef title,
    FTL::CStrRef initialCode,
    QPointF pos
    ) = 0;

  static FTL::CStrRef CmdName_AddInstFromPreset()
    { return FTL_STR("dfgAddInstFromPreset"); }
  virtual std::string dfgDoAddInstFromPreset(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef presetPath,
    QPointF pos
    ) = 0;

  static FTL::CStrRef CmdName_AddVar()
    { return FTL_STR("dfgAddVar"); }
  virtual std::string dfgDoAddVar(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredNodeName,
    FTL::CStrRef dataType,
    FTL::CStrRef extDep,
    QPointF pos
    ) = 0;

  static FTL::CStrRef CmdName_AddGet()
    { return FTL_STR("dfgAddGet"); }
  virtual std::string dfgDoAddGet(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredNodeName,
    FTL::CStrRef varPath,
    QPointF pos
    ) = 0;

  static FTL::CStrRef CmdName_AddSet()
    { return FTL_STR("dfgAddSet"); }
  virtual std::string dfgDoAddSet(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredNodeName,
    FTL::CStrRef varPath,
    QPointF pos
    ) = 0;

  static FTL::CStrRef CmdName_AddPort()
    { return FTL_STR("dfgAddPort"); }
  virtual std::string dfgDoAddPort(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredPortName,
    FabricCore::DFGPortType portType,
    FTL::CStrRef typeSpec,
    FTL::CStrRef portToConnect
    ) = 0;

  static FTL::CStrRef CmdName_RemovePort()
    { return FTL_STR("dfgRemovePort"); }
  virtual void dfgDoRemovePort(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef portName
    ) = 0;

  static FTL::CStrRef CmdName_MoveNodes()
    { return FTL_STR("dfgMoveNodes"); }
  virtual void dfgDoMoveNodes(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::ArrayRef<FTL::CStrRef> nodeNames,
    FTL::ArrayRef<QPointF> newTopLeftPoss
    ) = 0;

  static FTL::CStrRef CmdName_ResizeBackDropNode()
    { return FTL_STR("dfgResizeBackDropNode"); }
  virtual void dfgDoResizeBackDropNode(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef backDropNodeName,
    QPointF newTopLeftPos,
    QSizeF newSize
    ) = 0;

  static FTL::CStrRef CmdName_ImplodeNodes()
    { return FTL_STR("dfgImplodeNodes"); }
  virtual std::string dfgDoImplodeNodes(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef desiredNodeName,
    FTL::ArrayRef<FTL::CStrRef> nodeNames
    ) = 0;

  static FTL::CStrRef CmdName_ExplodeNode()
    { return FTL_STR("dfgExplodeNode"); }
  virtual std::vector<std::string> dfgDoExplodeNode(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef nodeName
    ) = 0;

  static FTL::CStrRef CmdName_AddBackDrop()
    { return FTL_STR("dfgAddBackDrop"); }
  virtual void dfgDoAddBackDrop(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef title,
    QPointF pos
    ) = 0;

  static FTL::CStrRef CmdName_SetNodeTitle()
    { return FTL_STR("dfgSetNodeTitle"); }
  virtual void dfgDoSetNodeTitle(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef nodeName,
    FTL::CStrRef newTitle
    ) = 0;

  static FTL::CStrRef CmdName_SetNodeComment()
    { return FTL_STR("dfgSetNodeComment"); }
  virtual void dfgDoSetNodeComment(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef nodeName,
    FTL::CStrRef comment
    ) = 0;

  static FTL::CStrRef CmdName_SetNodeCommentExpanded()
    { return FTL_STR("dfgSetNodeCommentExpanded"); }
  virtual void dfgDoSetNodeCommentExpanded(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef nodeName,
    bool expanded
    ) = 0;

  static FTL::CStrRef CmdName_SetCode()
    { return FTL_STR("dfgSetCode"); }
  virtual void dfgDoSetCode(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef code
    ) = 0;

  static FTL::CStrRef CmdName_RenameExecPort()
    { return FTL_STR("dfgRenameExecPort"); }
  virtual std::string dfgDoRenameExecPort(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef oldName,
    FTL::CStrRef desiredNewName
    ) = 0;

  static FTL::CStrRef CmdName_Paste()
    { return FTL_STR("dfgPaste"); }
  virtual std::vector<std::string> dfgDoPaste(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef json,
    QPointF cursorPos
    ) = 0;

  static FTL::CStrRef CmdName_SetArgType()
    { return FTL_STR("dfgSetArgType"); }
  virtual void dfgDoSetArgType(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef argName,
    FTL::CStrRef typeName
    ) = 0;

  static FTL::CStrRef CmdName_SetArgValue()
    { return FTL_STR("dfgSetArgValue"); }
  virtual void dfgDoSetArgValue(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef argName,
    FabricCore::RTVal const &value
    ) = 0;

  static FTL::CStrRef CmdName_SetPortDefaultValue()
    { return FTL_STR("dfgSetPortDefaultValue"); }
  virtual void dfgDoSetPortDefaultValue(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef portPath,
    FabricCore::RTVal const &value
    ) = 0;

  static FTL::CStrRef CmdName_SetRefVarPath()
    { return FTL_STR("dfgSetRefVarPath"); }
  virtual void dfgDoSetRefVarPath(
    FTL::CStrRef desc,
    FabricCore::DFGBinding const &binding,
    FTL::CStrRef execPath,
    FabricCore::DFGExec const &exec,
    FTL::CStrRef refName,
    FTL::CStrRef varPath
    ) = 0;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmdHandler__
