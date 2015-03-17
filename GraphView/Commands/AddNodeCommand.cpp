// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "AddNodeCommand.h"

using namespace FabricUI::GraphView;

AddNodeCommand::AddNodeCommand(Controller * controller, QString path, QString preset, QPointF pos)
: ControllerCommand(controller)
{
  m_path = path;
  m_preset = preset;
  m_graphPos = pos;
  m_useTopLeftPos = false;
}

AddNodeCommand::~AddNodeCommand()
{
}

Node * AddNodeCommand::getNode()
{
  return controller()->graph()->nodeFromPath(m_path);
}

bool AddNodeCommand::invoke()
{
  Node * node = controller()->graph()->addNodeFromPreset(m_path, m_preset);
  if(!node)
    return false;
  m_path = node->path();
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
