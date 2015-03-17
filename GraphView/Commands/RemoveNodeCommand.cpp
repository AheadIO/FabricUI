// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "RemoveNodeCommand.h"

using namespace FabricUI::GraphView;

RemoveNodeCommand::RemoveNodeCommand(Controller * controller, QString path)
: ControllerCommand(controller)
{
  m_path = path;
}

RemoveNodeCommand::~RemoveNodeCommand()
{
}

Node * RemoveNodeCommand::getNode()
{
  if(m_path.length() == 0)
    return NULL;
  return controller()->graph()->nodeFromPath(m_path);
}

bool RemoveNodeCommand::invoke()
{
  Node * node = getNode();
  if(!node)
    return false;

  m_preset = node->preset();
  m_graphPos = node->topLeftGraphPos();

  return controller()->graph()->removeNode(node);
}

bool RemoveNodeCommand::undo()
{
  Node * node = controller()->graph()->addNodeFromPreset(m_path, m_preset);
  if(!node)
    return false;
  m_path = node->path();
  controller()->moveNode(node, m_graphPos, true);
  return true;
}
