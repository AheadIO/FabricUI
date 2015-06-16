// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRemoveConnectionCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRemoveConnectionCommand::DFGRemoveConnectionCommand(DFGController * controller, const char * srcPath, const char * dstPath)
: DFGCommand(controller)
{
  m_srcPath = srcPath;
  m_dstPath = dstPath;
}

bool DFGRemoveConnectionCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  graph.disconnectFrom(m_srcPath.c_str(), m_dstPath.c_str());
  return true;
}

const char * DFGRemoveConnectionCommand::getSrcPath() const
{
  return m_srcPath.c_str();
}

const char * DFGRemoveConnectionCommand::getDstPath() const
{
  return m_dstPath.c_str();
}
