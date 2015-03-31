// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRemovePortCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRemovePortCommand::DFGRemovePortCommand(DFGController * controller, QString execPath, QString name)
: DFGCommand(controller)
{
  m_execPath = execPath.toUtf8().constData();
  m_portTitle = name.toUtf8().constData();
}

bool DFGRemovePortCommand::invoke()
{
  DFGView * view = (DFGView *)((DFGController*)controller())->getView();
  DFGWrapper::GraphExecutablePtr graph = view->getGraph();
  DFGWrapper::ExecutablePtr exec(graph);

  std::string remainingGraphPath = GraphView::relativePathSTL(graph->getGraphPath(), m_execPath);
  while(remainingGraphPath.length() > 0)
  {
    std::string nodeName = remainingGraphPath;
    int periodPos = nodeName.find('.');
    if(periodPos != std::string::npos)
      nodeName = nodeName.substr(0, periodPos);
    DFGWrapper::NodePtr node = graph->getNode(nodeName.c_str());
    exec = node->getExecutable();
    if(exec->getExecType() != FabricCore::DFGExecType_Graph)
      break;
    graph = DFGWrapper::GraphExecutablePtr::StaticCast(exec);
    remainingGraphPath = GraphView::relativePathSTL(graph->getGraphPath(), remainingGraphPath);
  }
  exec->removePort(m_portTitle.c_str());
  return true;
}

bool DFGRemovePortCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();  
}

bool DFGRemovePortCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();  
}

const char * DFGRemovePortCommand::getExecPath() const
{
  return m_execPath.c_str();
}

const char * DFGRemovePortCommand::getPortName() const
{
  return m_portTitle.c_str();
}
