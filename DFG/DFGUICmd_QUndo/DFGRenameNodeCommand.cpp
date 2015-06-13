// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRenameNodeCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRenameNodeCommand::DFGRenameNodeCommand(DFGController * controller, char const * path, char const * title)
: DFGCommand(controller)
{
  m_nodePath = path;
  m_newTitle = title;
}

bool DFGRenameNodeCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getExec();
  graph.setInstTitle(m_nodePath.c_str(), m_newTitle.c_str());
  return true;
}

char const * DFGRenameNodeCommand::getPath() const
{
  return m_nodePath.c_str();
}

char const * DFGRenameNodeCommand::getTitle() const
{
  return m_newTitle.c_str();
}
