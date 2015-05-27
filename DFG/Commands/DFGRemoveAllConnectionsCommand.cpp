// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRemoveAllConnectionsCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRemoveAllConnectionsCommand::DFGRemoveAllConnectionsCommand(DFGController * controller, const char * path, bool asPin)
: DFGCommand(controller)
{
  m_isPin = asPin;
  m_path = path;
  m_pinTitle = GraphView::lastPathSegmentSTL(m_path);
  m_nodePath = GraphView::parentPathSTL(m_path);
}

bool DFGRemoveAllConnectionsCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGView * view = ctrl->getView();
  DFGWrapper::GraphExecutablePtr graph = view->getGraph();

  if(m_isPin)
  {
    DFGWrapper::NodePtr node = ctrl->getNodeFromPath(m_nodePath.c_str());
    DFGWrapper::PinPtr pin = node->getPin(m_pinTitle.c_str());
    pin->disconnectFromAll();
  }
  else
  {
    DFGWrapper::ExecutablePtr exec = ctrl->getExecFromPath(m_nodePath.c_str());
    DFGWrapper::ExecPortPtr port = exec->getPort(m_pinTitle.c_str());
    port->disconnectFromAll();
  }
  return true;
}

bool DFGRemoveAllConnectionsCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();  
}

bool DFGRemoveAllConnectionsCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();  
}

const char * DFGRemoveAllConnectionsCommand::getPath() const
{
  return m_path.c_str();
}

bool DFGRemoveAllConnectionsCommand::isPin() const
{
  return m_isPin;
}
