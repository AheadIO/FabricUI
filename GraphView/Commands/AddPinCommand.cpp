// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "AddPinCommand.h"

using namespace FabricUI::GraphView;

AddPinCommand::AddPinCommand(Controller * controller, Node * node, QString name, PortType pType, QColor color, QString dataType)
: ControllerCommand(controller)
{
  m_path = node->path();
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
  return controller()->graph()->nodeFromPath(m_path);
}

Pin * AddPinCommand::getPin()
{
  return getNode()->pin(m_name);
}

bool AddPinCommand::invoke()
{
  Node * node = getNode();
  if(!node)
    return false;

  Pin * pin = new Pin(node, m_name, m_pType, m_color);
  pin->setDataType(m_dataType);

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
