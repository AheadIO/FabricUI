// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddSetCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddSetCommand::DFGAddSetCommand(
  DFGController * controller,
  FTL::StrRef name,
  FTL::StrRef path,
  QPointF pos
  )
  : DFGCommand(controller)
  , m_varName(name)
  , m_varPath(path)
  , m_pos(pos)
{
}

char const * DFGAddSetCommand::getVarName() const
{
  return m_varName.c_str();
}

char const * DFGAddSetCommand::getVarPath() const
{
  return m_varPath.c_str();
}

char const * DFGAddSetCommand::getNodePath() const
{
  return m_nodePath.c_str();
}

QPointF DFGAddSetCommand::getPos() const
{
  return m_pos;
}

GraphView::Node * DFGAddSetCommand::getNode()
{
  DFGController * ctrl = (DFGController*)controller();
  if(ctrl->graph())
    return ctrl->graph()->nodeFromPath(m_nodePath.c_str());
  return NULL;
}

bool DFGAddSetCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  m_nodePath = graph.addSet(m_varName.c_str(), m_varPath.c_str());
  ctrl->moveNode(m_nodePath.c_str(), m_pos, false);
  return true;
}
