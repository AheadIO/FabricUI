//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGUIPerform__
#define __UI_DFG_DFGUIPerform__

#include <FabricCore.h>
#include <FabricUI/DFG/DFGUINamespace.h>
#include <FTL/ArrayRef.h>
#include <FTL/CStrRef.h>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <string>
#include <vector>

FABRIC_UI_DFG_NAMESPACE_BEGIN

std::string DFGUIPerform_AddPort(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desiredPortName,
  FabricCore::DFGPortType dfgPortType,
  FTL::CStrRef typeSpec,
  FTL::CStrRef portToConnect,
  unsigned &coreUndoCount
  );

void DFGUIPerform_RemovePort(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef portName,
  unsigned &coreUndoCount
  );

void DFGUIPerform_MoveNodes(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::ArrayRef<std::string> nodeNames,
  FTL::ArrayRef<QPointF> newTopLeftPoss, 
  unsigned &coreUndoCount
  );

void DFGUIPerform_ResizeBackDropNode(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef backDropNodeName,
  QPointF newTopLeftPos,
  QSizeF newSize,
  unsigned &coreUndoCount
  );

std::string DFGUIPerform_ImplodeNodes(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desiredNodeName,
  FTL::ArrayRef<std::string> nodeNames,
  unsigned &coreUndoCount
  );

std::vector<std::string> DFGUIPerform_ExplodeNode(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  unsigned &coreUndoCount
  );

std::vector<std::string> DFGUIPerform_Paste(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef json,
  QPointF cursorPos,
  unsigned &coreUndoCount
  );

std::string DFGUIPerform_AddBackDrop(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef title,
  QPointF pos,
  unsigned &coreUndoCount
  );

void DFGUIPerform_SetNodeTitle(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  FTL::CStrRef newTitle,
  unsigned &coreUndoCount
  );

void DFGUIPerform_SetNodeComment(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  FTL::CStrRef comment,
  unsigned &coreUndoCount
  );

void DFGUIPerform_SetNodeCommentExpanded(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  bool expanded,
  unsigned &coreUndoCount
  );

void DFGUIPerform_SetCode(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef code,
  unsigned &coreUndoCount
  );

std::string DFGUIPerform_RenameExecPort(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef oldName,
  FTL::CStrRef desiredNewName,
  unsigned &coreUndoCount
  );

void DFGUIPerform_SetArgType(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef argName,
  FTL::CStrRef typeName,
  unsigned &coreUndoCount
  );

void DFGUIPerform_SetArgValue(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef argName,
  FabricCore::RTVal const &value,
  unsigned &coreUndoCount
  );

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUIPerform__
