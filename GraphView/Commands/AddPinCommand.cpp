// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "AddPinCommand.h"

using namespace FabricUI::GraphView;

AddPinCommand::AddPinCommand(Controller * controller, Node * node, char const * name, PortType pType, QColor color, char const * dataType)
: ControllerCommand(controller)
{
  m_path = node->name();
  m_name = name;
  m_pType = pType;
  m_color = color;
  m_dataType = dataType;
}

AddPinCommand::~AddPinCommand()
{
}

Node * AddPinCommand::getNode()
{
  return controller()->graph()->nodeFromPath(m_path.c_str());
}

Pin * AddPinCommand::getPin()
{
  return getNode()->pin(m_name.c_str());
}

bool AddPinCommand::invoke()
{
  Node * node = getNode();
  if(!node)
    return false;

  Pin * pin = new Pin(node, m_name.c_str(), m_pType, m_color);
  pin->setDataType(m_dataType.c_str());

  if(node->addPin(pin, false))
  {
    m_name = pin->name();
    return true;
  }

  delete(pin);
  return false;
}

bool AddPinCommand::undo()
{
  Pin * pin = getPin();
  if(!pin)
    return false;

  return getNode()->removePin(pin, false);
}
