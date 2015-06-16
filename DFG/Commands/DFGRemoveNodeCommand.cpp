// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRemoveNodeCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRemoveNodeCommand::DFGRemoveNodeCommand(DFGController * controller, char const * path)
: DFGCommand(controller)
{
  m_nodePath = path;
}

char const * DFGRemoveNodeCommand::getNodePath() const
{
  return m_nodePath.c_str();
}

bool DFGRemoveNodeCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  graph.removeNode(m_nodePath.c_str());
  return true;
}
