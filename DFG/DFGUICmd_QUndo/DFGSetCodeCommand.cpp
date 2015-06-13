// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGSetCodeCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGSetCodeCommand::DFGSetCodeCommand(DFGController * controller, char const * path, char const * code)
: DFGCommand(controller)
{
  m_execPath = path;
  m_code = code;
}

bool DFGSetCodeCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec exec = ctrl->getExec();

  if(exec.getType() != FabricCore::DFGExecType_Func)
    return false;

  exec.setCode(m_code.c_str());
  return true;
}

char const * DFGSetCodeCommand::getPath() const
{
  return m_execPath.c_str();
}

char const * DFGSetCodeCommand::getCode() const
{
  return m_code.c_str();
}
