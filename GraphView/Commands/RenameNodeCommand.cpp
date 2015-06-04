// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "RenameNodeCommand.h"

using namespace FabricUI::GraphView;

RenameNodeCommand::RenameNodeCommand(Controller * controller, Node * node, char const * title)
: ControllerCommand(controller)
{
  m_path = node->name();
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
  return controller()->graph()->nodeFromPath(m_path.c_str());
}

bool RenameNodeCommand::invoke()
{
  Node * node = getNode();
  if(!node)
    return false;
  node->setTitle(m_newTitle.c_str());
  return true;
}

bool RenameNodeCommand::undo()
{
  Node * node = getNode();
  if(!node)
    return false;
  node->setTitle(m_oldTitle.c_str());
  return true;
}
