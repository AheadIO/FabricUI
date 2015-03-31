// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddPortCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddPortCommand::DFGAddPortCommand(DFGController * controller, QString execPath, QString name, GraphView::PortType pType, QString dataType)
: DFGCommand(controller)
{
  m_execPath = execPath.toUtf8().constData();
  m_portTitle = name.toUtf8().constData();
  m_dataType = dataType.toUtf8().constData();
  m_portType = pType;
}


bool DFGAddPortCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGWrapper::ExecutablePtr exec = ctrl->getExecFromPath(m_execPath.c_str());

  FabricCore::DFGPortType portType = FabricCore::DFGPortType_In;
  if(m_portType == GraphView::PortType_Input)
    portType = FabricCore::DFGPortType_Out;
  else if(m_portType == GraphView::PortType_IO)
    portType = FabricCore::DFGPortType_IO;

  m_portPath = exec->addPort(m_portTitle.c_str(), portType, m_dataType.length() > 0 ? m_dataType.c_str() : NULL)->getEndPointPath();
  return true;
}

bool DFGAddPortCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();  
}

bool DFGAddPortCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();  
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

