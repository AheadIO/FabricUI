// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRenamePortCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRenamePortCommand::DFGRenamePortCommand(DFGController * controller, char const * path, char const * name)
: DFGCommand(controller)
{
  m_path = path;
  m_newName = name;
}

bool DFGRenamePortCommand::invoke()
{
  DFGController* ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getExec();
  m_result = graph.renameExecPort(m_path.c_str(), m_newName.c_str());
  return true;
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
