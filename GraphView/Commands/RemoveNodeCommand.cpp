// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "RemoveNodeCommand.h"

using namespace FabricUI::GraphView;

RemoveNodeCommand::RemoveNodeCommand(
  Controller * controller,
  FTL::StrRef name
  )
  : ControllerCommand(controller)
  , m_name( name )
{
}

RemoveNodeCommand::~RemoveNodeCommand()
{
}

Node * RemoveNodeCommand::getNode()
{
  return controller()->graph()->nodeFromPath( m_name );
}

bool RemoveNodeCommand::invoke()
{
  Node * node = getNode();
  m_title = node->title();
  m_graphPos = node->topLeftGraphPos();
  return controller()->graph()->removeNode(node);
}

bool RemoveNodeCommand::undo()
{
  Node * node = controller()->graph()->addNode(m_name, m_title);
  controller()->moveNode(node, m_graphPos, true);
  return true;
}
