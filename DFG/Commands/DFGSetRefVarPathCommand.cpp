// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGSetRefVarPathCommand.h"

#include <FabricUI/GraphView/Graph.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGSetRefVarPathCommand::DFGSetRefVarPathCommand(DFGController * controller, char const * path, char const * varPath)
: DFGCommand(controller)
{
  m_path = path;
  m_varPath = varPath;
}

bool DFGSetRefVarPathCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec parentExec = ctrl->getCoreDFGBinding().getExec();

  std::string path = m_path;

  // for paths with more than one segment,
  // get the corresponding subexec
  // @pzion: should setRefVarPath / getRefVarPath
  // automatically do this?
  int pos = path.find('.');
  while(pos != std::string::npos)
  {
    std::string left = path.substr(0, pos);
    path = path.substr(pos+1);
    parentExec = parentExec.getSubExec(left.c_str());
    pos = path.find('.');
  }

  parentExec.setRefVarPath(path.c_str(), m_varPath.c_str());
  return true;
}

const char * DFGSetRefVarPathCommand::getPath() const
{
  return m_path.c_str();
}

const char * DFGSetRefVarPathCommand::getVarPath() const
{
  return m_varPath.c_str();
}
