// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/Commands/AddConnectionCommand.h>

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
    return panel->port(m_srcName.c_str());
  }

  Node * node = controller()->graph()->node(m_srcPath.c_str());
  if(!node)
    return NULL;
  return node->pin(m_srcName.c_str());
}

ConnectionTarget * AddConnectionCommand::getDstTarget()
{
  if(m_dstPath == "")
  {
    SidePanel * panel = controller()->graph()->sidePanel(PortType_Input);
    if(!panel)
      return NULL;
    return panel->port(m_dstName.c_str());
  }

  Node * node = controller()->graph()->node(m_dstPath.c_str());
  if(!node)
    return NULL;
  return node->pin(m_dstName.c_str());
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
