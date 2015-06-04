// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddPortCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddPortCommand::DFGAddPortCommand(DFGController * controller, char const * execPath, char const * name, GraphView::PortType pType, char const * dataType)
: DFGCommand(controller)
{
  m_execPath = execPath;
  m_portTitle = name;
  m_dataType = dataType;
  m_portType = pType;
}


bool DFGAddPortCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec exec = ctrl->getCoreDFGExec();

  FabricCore::DFGPortType portType = FabricCore::DFGPortType_In;
  if(m_portType == GraphView::PortType_Input)
    portType = FabricCore::DFGPortType_Out;
  else if(m_portType == GraphView::PortType_IO)
    portType = FabricCore::DFGPortType_IO;

  m_portPath = exec.addExecPort(m_portTitle.c_str(), portType, m_dataType.length() > 0 ? m_dataType.c_str() : NULL);
  return true;
}

const char * DFGAddPortCommand::getExecPath() const
{
  return m_execPath.c_str();
}

const char * DFGAddPortCommand::getPortName() const
{
  return m_portTitle.c_str();
}

GraphView::PortType DFGAddPortCommand::getPortType() const
{
  return m_portType;
}

const char * DFGAddPortCommand::getDataType() const
{
  return m_dataType.c_str();
}

const char * DFGAddPortCommand::getPortPath() const
{
  return m_portPath.c_str();
}

