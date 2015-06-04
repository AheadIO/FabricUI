// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddEmptyFuncCommand.h"
#include <DFGWrapper/Inst.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddEmptyFuncCommand::DFGAddEmptyFuncCommand(DFGController * controller, char const * path, char const * title, QPointF pos)
: DFGCommand(controller)
{
  m_path = path;
  m_title = title;
  m_pos = pos;
}

char const * DFGAddEmptyFuncCommand::getPath() const
{
  return m_path.c_str();
}

char const * DFGAddEmptyFuncCommand::getTitle() const
{
  return m_title.c_str();
}

QPointF DFGAddEmptyFuncCommand::getPos() const
{
  return m_pos;
}

char const * DFGAddEmptyFuncCommand::getInstPath() const
{
  return m_instPath.c_str();
}

GraphView::Node * DFGAddEmptyFuncCommand::getNode()
{
  DFGController * ctrl = (DFGController*)controller();
  if(!ctrl->graph())
    return NULL;
  return ctrl->graph()->nodeFromPath(m_instPath.c_str());
}

bool DFGAddEmptyFuncCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  m_instPath = graph.addInstWithNewFunc(m_title.c_str());
  graph.setInstTitle(m_instPath.c_str(), m_title.c_str());
  if(ctrl->graph())
  {
    GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_instPath.c_str());
    if(uiNode)
      ctrl->moveNode(uiNode, m_pos, false);
  }
  return true;
}
