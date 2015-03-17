// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <DFGWrapper/FuncExecutable.h>
#include "DFGSetCodeCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGSetCodeCommand::DFGSetCodeCommand(DFGController * controller, QString path, QString code)
: DFGCommand(controller)
{
  m_execPath = path.toUtf8().constData();
  m_code = code.toUtf8().constData();
}

bool DFGSetCodeCommand::invoke()
{
  DFGView * view = (DFGView *)((DFGController*)controller())->getView();
  DFGWrapper::GraphExecutable graph = view->getGraph();
  DFGWrapper::Executable exec(graph);
  std::string remainingGraphPath = GraphView::relativePathSTL(graph.getPath(), m_execPath);
  while(remainingGraphPath.length() > 0)
  {
    std::string nodeName = remainingGraphPath;
    int periodPos = nodeName.find('.');
    if(periodPos != std::string::npos)
      nodeName = nodeName.substr(0, periodPos);
    DFGWrapper::Node node = graph.getNode(nodeName.c_str());
    exec = node.getExecutable();
    if(exec.getObjectType() != "Graph")
      break;
    graph = exec;
    remainingGraphPath = GraphView::relativePathSTL(graph.getPath(), remainingGraphPath);
  }

  if(exec.getObjectType() != "Func")
    return false;

  DFGWrapper::FuncExecutable func(exec);
  func.setCode(m_code.c_str());
  return true;
}

bool DFGSetCodeCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();  
}

bool DFGSetCodeCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();  
}

std::string DFGSetCodeCommand::getPath() const
{
  return m_execPath;
}

std::string DFGSetCodeCommand::getCode() const
{
  return m_code;
}
