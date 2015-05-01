// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGExplodeNodeCommand.h"
#include <QtGui/QApplication>
#include <QtGui/QClipboard>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGExplodeNodeCommand::DFGExplodeNodeCommand(DFGController * controller, QString nodePath)
: DFGCommand(controller)
{
  m_nodePath = nodePath.toUtf8().constData();
}

std::string DFGExplodeNodeCommand::getNodePath() const
{
  return m_nodePath;
}

std::vector<std::string> DFGExplodeNodeCommand::getNodeNames() const
{
  return m_nodeNames;
}

bool DFGExplodeNodeCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGWrapper::GraphExecutablePtr graph = ctrl->getGraphExec();

  m_nodeNames = graph->explodeNode(m_nodePath.c_str());

  return true;
}

bool DFGExplodeNodeCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();
}

bool DFGExplodeNodeCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();
}
