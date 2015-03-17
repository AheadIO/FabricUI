// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "RenameNodeCommand.h"

using namespace FabricUI::GraphView;

RenameNodeCommand::RenameNodeCommand(Controller * controller, Node * node, QString title)
: ControllerCommand(controller)
{
  m_path = node->path();
  m_newTitle = title;
  m_oldTitle = node->title();
}

RenameNodeCommand::~RenameNodeCommand()
{
}

Node * RenameNodeCommand::getNode()
{
  if(m_path.length() == 0)
    return NULL;
  return controller()->graph()->nodeFromPath(m_path);
}

bool RenameNodeCommand::invoke()
{
  Node * node = getNode();
  if(!node)
    return false;
  node->setTitle(m_newTitle);
  return true;
}

bool RenameNodeCommand::undo()
{
  Node * node = getNode();
  if(!node)
    return false;
  node->setTitle(m_oldTitle);
  return true;
}
