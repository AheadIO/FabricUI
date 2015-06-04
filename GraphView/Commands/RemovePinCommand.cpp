// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "RemovePinCommand.h"

using namespace FabricUI::GraphView;

RemovePinCommand::RemovePinCommand(Controller * controller, Pin * pin)
: ControllerCommand(controller)
{
  m_path = pin->node()->name();
  m_name = pin->name();
  m_pType = pin->portType();
  m_color = pin->color();
  m_dataType = pin->dataType();
}

RemovePinCommand::~RemovePinCommand()
{
}

Node * RemovePinCommand::getNode()
{
  return controller()->graph()->nodeFromPath(m_path.c_str());
}

Pin * RemovePinCommand::getPin()
{
  return getNode()->pin(m_name.c_str());
}

bool RemovePinCommand::invoke()
{
  Node * node = getNode();
  if(!node)
    return false;

  Pin * pin = getPin();
  if(!pin)
    return false;

  return getNode()->removePin(pin, false);
}

bool RemovePinCommand::undo()
{
  Node * node = getNode();
  if(!node)
    return false;

  Pin * pin = new Pin(node, m_name.c_str(), m_pType, m_color);
  pin->setDataType(m_dataType.c_str());

  if(node->addPin(pin, false))
    return true;

  delete(pin);
  return false;
}
