// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "RenamePortCommand.h"

using namespace FabricUI::GraphView;

RenamePortCommand::RenamePortCommand(Controller * controller, Port * port, QString title)
: ControllerCommand(controller)
{
  m_path = port->path();
  m_portType = port->portType();
  m_oldTitle = port->name();
  m_newTitle = title;
}

RenamePortCommand::~RenamePortCommand()
{
}

Port * RenamePortCommand::getPort()
{
  SidePanel * panel = controller()->graph()->sidePanel(m_portType);
  if(!panel)
    return NULL;

  return panel->port(m_path);
}

bool RenamePortCommand::invoke()
{
  Port * port = getPort();
  if(!port)
    return false;
  port->setLabel(m_newTitle);
  return true;
}

bool RenamePortCommand::undo()
{
  Port * port = getPort();
  if(!port)
    return false;
  port->setLabel(m_oldTitle);
  return true;
}
