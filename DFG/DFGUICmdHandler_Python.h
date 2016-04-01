//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGUICmdHandler_Python__
#define __UI_DFG_DFGUICmdHandler_Python__

#include <FabricUI/DFG/DFGUICmdHandler.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class UndoCmd_Python
{
private:
  UndoCmd_Python()
  {
  }

public:

  static void TryCatchDoit( DFGUICmd *cmd )
  {
    try
    {
      cmd->doit();
    }
    catch ( FabricCore::Exception e )
    {
      // [andrew 20160321] FIXME FE-6332
      printf( "Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
    }
  }

  static void TryCatchRedo( DFGUICmd *cmd )
  {
    try
    {
      cmd->redo();
    }
    catch ( FabricCore::Exception e )
    {
      // [andrew 20160321] FIXME FE-6332
      printf( "Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
    }
  }

  static void TryCatchUndo( DFGUICmd *cmd )
  {
    try
    {
      cmd->undo();
    }
    catch ( FabricCore::Exception e )
    {
      // [andrew 20160321] FIXME FE-6332
      printf( "Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
    }

  }
};

class DFGUICmdHandler_Python : public DFGUICmdHandler
{
public:

  DFGUICmdHandler_Python() {}
  virtual ~DFGUICmdHandler_Python() {}

  virtual void dfgDoRemoveNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames
    ) {}

  virtual void dfgDoConnect(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString srcPath, 
    QString dstPath
    ) {}

  virtual void dfgDoDisconnect(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString srcPath, 
    QString dstPath
    ) {}

  virtual QString dfgDoAddGraph(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString title,
    QPointF pos
    ) { return QString(); }

  virtual QString dfgDoAddFunc(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString title,
    QString initialCode,
    QPointF pos
    ) { return QString(); }

  virtual QString dfgDoInstPreset(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString presetPath,
    QPointF pos
    ) { return QString(); }

  virtual QString dfgDoAddVar(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString dataType,
    QString extDep,
    QPointF pos
    ) { return QString(); }

  virtual QString dfgDoAddGet(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString varPath,
    QPointF pos
    ) { return QString(); }

  virtual QString dfgDoAddSet(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredNodeName,
    QString varPath,
    QPointF pos
    ) { return QString(); }

  virtual QString dfgDoAddPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredPortName,
    FabricCore::DFGPortType portType,
    QString typeSpec,
    QString portToConnect,
    QString extDep,
    QString metaData
    ) { return QString(); }

  virtual QString dfgDoCreatePreset(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString presetDirPath,
    QString presetName
    ) { return QString(); }

  virtual QString dfgDoEditPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString oldPortName,
    QString desiredNewPortName,
    QString typeSpec,
    QString extDep,
    QString uiMetadata
    ) { return QString(); }

  virtual void dfgDoRemovePort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString portName
    ) {}

  virtual void dfgDoMoveNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames,
    QList<QPointF> newTopLeftPoss
    ) {}

  virtual void dfgDoResizeBackDrop(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString backDropNodeName,
    QPointF newTopLeftPos,
    QSizeF newSize
    ) {}

  virtual QString dfgDoImplodeNodes(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList nodeNames,
    QString desiredNodeName
    ) { return QString(); }

  virtual QStringList dfgDoExplodeNode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName
    ) { return QStringList(); }

  virtual void dfgDoAddBackDrop(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString title,
    QPointF pos
    ) {}

  virtual void dfgDoSetNodeComment(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString comment
    ) {}

  virtual void dfgDoSetCode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString code
    ) {}

  virtual QString dfgDoEditNode(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString oldNodeName,
    QString desiredNewNodeName,
    QString nodeMetadata,
    QString execMetadata
    ) { return QString(); }

  virtual QString dfgDoRenamePort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString oldPortName,
    QString desiredNewPortName
    ) { return QString(); }

  virtual QStringList dfgDoPaste(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString json,
    QPointF cursorPos
    ) { return QStringList(); }

  virtual void dfgDoSetArgValue(
    FabricCore::DFGBinding const &binding,
    QString argName,
    FabricCore::RTVal const &value
    ) {}

  virtual void dfgDoSetPortDefaultValue(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString portPath,
    FabricCore::RTVal const &value
    ) {}

  virtual void dfgDoSetRefVarPath(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString refName,
    QString varPath
    ) {}

  virtual void dfgDoReorderPorts(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QList<int> indices
    ) {}

  virtual void dfgDoSetExtDeps(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList extDeps
    ) {}

  virtual void dfgDoSplitFromPreset(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec
    ) {}
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmdHandler_Python__
