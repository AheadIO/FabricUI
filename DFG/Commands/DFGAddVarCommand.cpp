// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddVarCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddVarCommand::DFGAddVarCommand(
  DFGController * controller,
  FTL::StrRef name,
  QPointF pos
  )
  : DFGCommand(controller)
  , m_varName(name)
  , m_pos(pos)
{
}

char const * DFGAddVarCommand::getVarName() const
{
  return m_varName.c_str();
}

char const * DFGAddVarCommand::getNodePath() const
{
  return m_nodePath.c_str();
}

QPointF DFGAddVarCommand::getPos() const
{
  return m_pos;
}

GraphView::Node * DFGAddVarCommand::getNode()
{
  DFGController * ctrl = (DFGController*)controller();
  if(ctrl->graph())
    return ctrl->graph()->nodeFromPath(m_nodePath.c_str());
  return NULL;
}

bool DFGAddVarCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  m_nodePath = graph.addVar(m_varName.c_str());
  ctrl->moveNode(m_nodePath.c_str(), m_pos, false);
  return true;
}
