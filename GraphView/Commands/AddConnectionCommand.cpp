// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "AddConnectionCommand.h"

using namespace FabricUI::GraphView;

AddConnectionCommand::AddConnectionCommand(Controller * controller, ConnectionTarget * src, ConnectionTarget * dst)
: ControllerCommand(controller)
{
  m_srcIsPort = src->targetType() == TargetType_Port;
  if(m_srcIsPort)
  {
    m_srcPath = "";
    m_srcName = ((Port*)src)->name();
  }
  else
  {
    m_srcPath = ((Pin*)src)->node()->path();
    m_srcName = ((Pin*)src)->name();
  }

  m_dstIsPort = dst->targetType() == TargetType_Port;
  if(m_dstIsPort)
  {
    m_dstPath = "";
    m_dstName = ((Port*)dst)->name();
  }
  else
  {
    m_dstPath = ((Pin*)dst)->node()->path();
    m_dstName = ((Pin*)dst)->name();
  }
}

AddConnectionCommand::~AddConnectionCommand()
{
}

ConnectionTarget * AddConnectionCommand::getSrcTarget()
{
  if(m_srcPath == "")
  {
    SidePanel * panel = controller()->graph()->sidePanel(PortType_Output);
    if(!panel)
      return NULL;
    return panel->port(m_srcName);
  }

  Node * node = controller()->graph()->nodeFromPath(m_srcPath);
  if(!node)
    return NULL;
  return node->pin(m_srcName);
}

ConnectionTarget * AddConnectionCommand::getDstTarget()
{
  if(m_dstPath == "")
  {
    SidePanel * panel = controller()->graph()->sidePanel(PortType_Input);
    if(!panel)
      return NULL;
    return panel->port(m_dstName);
  }

  Node * node = controller()->graph()->nodeFromPath(m_dstPath);
  if(!node)
    return NULL;
  return node->pin(m_dstName);
}

bool AddConnectionCommand::invoke()
{
  ConnectionTarget * src = getSrcTarget();
  ConnectionTarget * dst = getDstTarget();
  if(!src || !dst)
    return false;

  return controller()->graph()->addConnection(src, dst, false);
}

bool AddConnectionCommand::undo()
{
  ConnectionTarget * src = getSrcTarget();
  ConnectionTarget * dst = getDstTarget();
  if(!src || !dst)
    return false;

  return controller()->graph()->removeConnection(src, dst, false);
}
