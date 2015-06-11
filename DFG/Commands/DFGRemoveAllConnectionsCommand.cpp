// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRemoveAllConnectionsCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRemoveAllConnectionsCommand::DFGRemoveAllConnectionsCommand(DFGController * controller, const char * path)
: DFGCommand(controller)
{
  m_path = path;
}

bool DFGRemoveAllConnectionsCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  graph.disconnectFromAll(m_path.c_str());
  return true;
}

const char * DFGRemoveAllConnectionsCommand::getPath() const
{
  return m_path.c_str();
}
