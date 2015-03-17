// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRemoveNodeCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRemoveNodeCommand::DFGRemoveNodeCommand(DFGController * controller, QString path)
: DFGCommand(controller)
{
  m_nodePath = path;
}

QString DFGRemoveNodeCommand::getNodePath() const
{
  return m_nodePath;
}

bool DFGRemoveNodeCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGWrapper::GraphExecutable graph = ctrl->getGraphExecFromPath(GraphView::parentPath(m_nodePath).toUtf8().constData());
  graph.removeNode(ctrl->getNodeFromPath(m_nodePath.toUtf8().constData()));
  return true;
}

bool DFGRemoveNodeCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();
}

bool DFGRemoveNodeCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();  
}
