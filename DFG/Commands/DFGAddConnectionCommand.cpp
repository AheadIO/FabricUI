// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddConnectionCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddConnectionCommand::DFGAddConnectionCommand(DFGController * controller, const char * srcPath, const char * dstPath)
: DFGCommand(controller)
{
  m_srcPath = srcPath;
  m_dstPath = dstPath;
}

bool DFGAddConnectionCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  graph.connectTo(m_srcPath.c_str(), m_dstPath.c_str());
  return true;
}

const char * DFGAddConnectionCommand::getSrcPath() const
{
  return m_srcPath.c_str();
}

const char * DFGAddConnectionCommand::getDstPath() const
{
  return m_dstPath.c_str();
}
