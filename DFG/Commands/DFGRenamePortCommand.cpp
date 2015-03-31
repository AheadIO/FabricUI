// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRenamePortCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRenamePortCommand::DFGRenamePortCommand(DFGController * controller, QString path, QString name)
: DFGCommand(controller)
{
  m_path = path.toUtf8().constData();
  m_newName = name.toUtf8().constData();
}

bool DFGRenamePortCommand::invoke()
{
  DFGController* ctrl = (DFGController*)controller();
  std::string execPath = GraphView::parentPathSTL(m_path);
  std::string oldName = GraphView::lastPathSegmentSTL(m_path);
  DFGWrapper::ExecutablePtr exec = ctrl->getExecFromPath(execPath);
  DFGWrapper::PortPtr port = exec->getPort(oldName.c_str());
  m_result = port->rename(m_newName.c_str());
  return true;
}

bool DFGRenamePortCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();  
}

bool DFGRenamePortCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();  
}

const char * DFGRenamePortCommand::getPath() const
{
  return m_path.c_str();
}

const char * DFGRenamePortCommand::getPortName() const
{
  return m_newName.c_str();
}

const char * DFGRenamePortCommand::getResult() const
{
  return m_result.c_str();
}
