// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRemovePortCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRemovePortCommand::DFGRemovePortCommand(DFGController * controller, char const * name)
: DFGCommand(controller)
{
  m_portName = name;
}

bool DFGRemovePortCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  graph.removeExecPort(m_portName.c_str());
  return true;
}

const char * DFGRemovePortCommand::getPortName() const
{
  return m_portName.c_str();
}
