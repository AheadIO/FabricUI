// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/Commands/AddNodeCommand.h>

using namespace FabricUI::GraphView;

AddNodeCommand::AddNodeCommand(
  Controller * controller,
  FTL::CStrRef name,
  FTL::CStrRef title,
  QPointF pos
  )
  : ControllerCommand(controller)
  , m_name( name )
  , m_title( title )
  , m_graphPos( pos )
  , m_useTopLeftPos( false )
{
}

AddNodeCommand::~AddNodeCommand()
{
}

Node * AddNodeCommand::getNode()
{
  return controller()->graph()->nodeFromPath(m_name);
}

bool AddNodeCommand::invoke()
{
  Node * node = controller()->graph()->addNode(m_name, m_title);
  if(!node)
    return false;
  m_name = node->name();
  controller()->moveNode(node, m_graphPos, m_useTopLeftPos);
  return true;
}

bool AddNodeCommand::undo()
{
  Node * node = getNode();
  if(!node)
    return false;

  m_graphPos = node->topLeftGraphPos();
  m_useTopLeftPos = true;
  return controller()->graph()->removeNode(node);
}
