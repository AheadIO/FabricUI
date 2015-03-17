// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ControllerCommand.h"

using namespace FabricUI::GraphView;

ControllerCommand::ControllerCommand(Controller * controller)
{
  m_controller = controller;
}

ControllerCommand::~ControllerCommand()
{
}

Controller * ControllerCommand::controller()
{
  return m_controller;
}
