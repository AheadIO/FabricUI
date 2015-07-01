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

std::string DFGUIPerform_ImplodeNodes(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desiredNodeName,
  FTL::ArrayRef<FTL::CStrRef> const &nodeNames,
  unsigned &coreUndoCount
  );

std::vector<std::string> DFGUIPerform_ExplodeNode(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  unsigned &coreUndoCount
  );

void DFGUIPerform_MoveNode(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  QPoint pos,
  unsigned &coreUndoCount
  );

void DFGUIPerform_AddBackDrop(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef title,
  unsigned &coreUndoCount
  );

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUIPerform__
