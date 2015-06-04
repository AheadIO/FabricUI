// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "RemovePortCommand.h"

using namespace FabricUI::GraphView;

RemovePortCommand::RemovePortCommand(Controller * controller, Port * port)
: ControllerCommand(controller)
{
  m_name = port->name();
  m_pType = port->portType();
  m_color = port->color();
  m_dataType = port->dataType();
}

RemovePortCommand::~RemovePortCommand()
{
}

Port * RemovePortCommand::getPort()
{
  SidePanel * panel = controller()->graph()->sidePanel(m_pType);
  if(!panel)
    return NULL;

  return panel->port(m_name.c_str());
}

bool RemovePortCommand::invoke()
{
  SidePanel * panel = controller()->graph()->sidePanel(m_pType);
  if(!panel)
    return NULL;

  Port * port = getPort();
  if(!port)
    return false;

  return panel->removePort(port);
}

bool RemovePortCommand::undo()
{
  SidePanel * panel = controller()->graph()->sidePanel(m_pType);
  if(!panel)
    return NULL;

  Port * port = new Port(panel, m_name.c_str(), m_pType, m_dataType.c_str(), m_color);
  port->setDataType(m_dataType.c_str());

  if(panel->addPort(port))
    return true;

  delete(port);
  return false;
}
