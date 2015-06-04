// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "RemoveConnectionCommand.h"

using namespace FabricUI::GraphView;

RemoveConnectionCommand::RemoveConnectionCommand(Controller * controller, ConnectionTarget * src, ConnectionTarget * dst)
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
    m_srcPath = ((Pin*)src)->node()->name();
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
    m_dstPath = ((Pin*)dst)->node()->name();
    m_dstName = ((Pin*)dst)->name();
  }
}

RemoveConnectionCommand::~RemoveConnectionCommand()
{
}

ConnectionTarget * RemoveConnectionCommand::getSrcTarget()
{
  if(m_srcPath == "")
  {
    SidePanel * panel = controller()->graph()->sidePanel(PortType_Output);
    if(!panel)
      return NULL;
    return panel->port(m_srcName.c_str());
  }

  Node * node = controller()->graph()->nodeFromPath(m_srcPath.c_str());
  if(!node)
    return NULL;
  return node->pin(m_srcName.c_str());
}

ConnectionTarget * RemoveConnectionCommand::getDstTarget()
{
  if(m_dstPath == "")
  {
    SidePanel * panel = controller()->graph()->sidePanel(PortType_Input);
    if(!panel)
      return NULL;
    return panel->port(m_dstName.c_str());
  }

  Node * node = controller()->graph()->nodeFromPath(m_dstPath.c_str());
  if(!node)
    return NULL;
  return node->pin(m_dstName.c_str());
}

bool RemoveConnectionCommand::invoke()
{
  ConnectionTarget * src = getSrcTarget();
  ConnectionTarget * dst = getDstTarget();
  if(!src || !dst)
    return false;

  return controller()->graph()->removeConnection(src, dst, false);
}

bool RemoveConnectionCommand::undo()
{
  ConnectionTarget * src = getSrcTarget();
  ConnectionTarget * dst = getDstTarget();
  if(!src || !dst)
    return false;

  return controller()->graph()->addConnection(src, dst, false);
}
