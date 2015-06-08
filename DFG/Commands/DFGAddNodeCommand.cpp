// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddNodeCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddNodeCommand::DFGAddNodeCommand(DFGController * controller, char const * path, char const * preset, QPointF pos)
: DFGCommand(controller)
{
  m_path = path;
  m_preset = preset;
  m_pos = pos;
}

char const * DFGAddNodeCommand::getPath() const
{
  return m_path.c_str();
}

char const * DFGAddNodeCommand::getInstPath() const
{
  return m_instPath.c_str();
}

char const * DFGAddNodeCommand::getPreset() const
{
  return m_preset.c_str();
}

QPointF DFGAddNodeCommand::getPos() const
{
  return m_pos;
}

GraphView::Node * DFGAddNodeCommand::getNode()
{
  DFGController * ctrl = (DFGController*)controller();
  if(ctrl->graph())
    return ctrl->graph()->nodeFromPath(m_instPath.c_str());
  return NULL;
}

bool DFGAddNodeCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  m_instPath = graph.addInstFromPreset(m_preset.c_str());
  ctrl->moveNode(m_instPath.c_str(), m_pos, false);
  return true;
}
