// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddEmptyGraphCommand.h"
#include <DFGWrapper/Inst.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddEmptyGraphCommand::DFGAddEmptyGraphCommand(DFGController * controller, char const * path, char const * title, QPointF pos)
: DFGCommand(controller)
{
  m_path = path;
  m_title = title;
  m_pos = pos;
}

char const * DFGAddEmptyGraphCommand::getPath() const
{
  return m_path.c_str();
}

char const * DFGAddEmptyGraphCommand::getTitle() const
{
  return m_title.c_str();
}

QPointF DFGAddEmptyGraphCommand::getPos() const
{
  return m_pos;
}

char const * DFGAddEmptyGraphCommand::getInstPath() const
{
  return m_instPath.c_str();
}

GraphView::Node * DFGAddEmptyGraphCommand::getNode()
{
  DFGController * ctrl = (DFGController*)controller();
  if(!ctrl->graph())
    return NULL;
  return ctrl->graph()->nodeFromPath(m_instPath.c_str());
}

bool DFGAddEmptyGraphCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  m_instPath = graph.addInstWithNewGraph(m_title.c_str());
  graph.setInstTitle(m_instPath.c_str(), m_title.c_str());
  if(ctrl->graph())
  {
    GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_instPath.c_str());
    if(uiNode)
      ctrl->moveNode(uiNode, m_pos, false);
  }
  return true;
}
