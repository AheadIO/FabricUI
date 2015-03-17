// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "AddPortCommand.h"

using namespace FabricUI::GraphView;

AddPortCommand::AddPortCommand(Controller * controller, QString name, PortType pType, QColor color, QString dataType)
: ControllerCommand(controller)
{
  m_name = name;
  m_pType = pType;
  m_color = color;
  m_dataType = dataType;
}

AddPortCommand::~AddPortCommand()
{
}

Port * AddPortCommand::getPort()
{
  SidePanel * panel = controller()->graph()->sidePanel(m_pType);
  if(!panel)
    return NULL;
  return panel->port(m_name);
}

bool AddPortCommand::invoke()
{
  SidePanel * panel = controller()->graph()->sidePanel(m_pType);
  if(!panel)
    return false;

  Port * port = new Port(panel, m_name, m_pType, m_dataType, m_color);
  if(panel->addPort(port))
  {
    m_name = port->name();
    return true;
  }

  delete(port);
  return false;
}

bool AddPortCommand::undo()
{
  SidePanel * panel = controller()->graph()->sidePanel(m_pType);
  if(!panel)
    return false;

  Port * port = getPort();
  if(!port)
    return false;

  return panel->removePort(port);
}
