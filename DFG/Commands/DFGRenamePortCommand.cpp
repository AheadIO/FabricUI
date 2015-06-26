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
  FabricCore::DFGExec exec = ctrl->getCoreDFGExec();
  if(m_path.find('.') != std::string::npos)
  {
    std::string execName = m_path.substr(0, m_path.find('.'));
    std::string portName = m_path.substr(m_path.find('.')+1);
    if(exec.getType() == FabricCore::DFGExecType_Graph)
      exec = exec.getSubExec(execName.c_str());
    m_result = exec.renameExecPort(portName.c_str(), m_newName.c_str());
  }
  else
    m_result = exec.renameExecPort(m_path.c_str(), m_newName.c_str());
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
