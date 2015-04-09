// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddEmptyGraphCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddEmptyGraphCommand::DFGAddEmptyGraphCommand(DFGController * controller, QString path, QString title, QPointF pos)
: DFGCommand(controller)
{
  m_path = path.toUtf8().constData();
  m_title = title.toUtf8().constData();
  m_pos = pos;
}

std::string DFGAddEmptyGraphCommand::getPath() const
{
  return m_path;
}

std::string DFGAddEmptyGraphCommand::getTitle() const
{
  return m_title;
}

QPointF DFGAddEmptyGraphCommand::getPos() const
{
  return m_pos;
}

std::string DFGAddEmptyGraphCommand::getNodePath() const
{
  return m_nodePath;
}

GraphView::Node * DFGAddEmptyGraphCommand::getNode()
{
  DFGController * ctrl = (DFGController*)controller();
  if(!ctrl->graph())
    return NULL;
  return ctrl->graph()->nodeFromPath(m_nodePath.c_str());
}

bool DFGAddEmptyGraphCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGWrapper::GraphExecutablePtr graph = ctrl->getGraphExec();
  DFGWrapper::NodePtr node = graph->addNodeWithNewGraph(m_title.c_str());
  m_nodePath = node->getNodePath();
  if(ctrl->graph())
  {
    GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_nodePath.c_str());
    if(uiNode)
      ctrl->moveNode(uiNode, m_pos, false);
  }
  return true;
}

bool DFGAddEmptyGraphCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  if(ctrl->getHost()->maybeUndo())
  {
    if(ctrl->graph())
    {
      GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_nodePath.c_str());
      if(uiNode)
      {
        m_pos = uiNode->topLeftGraphPos();
      }
    }
    return true;
  }
  return false;
}

bool DFGAddEmptyGraphCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  if(ctrl->getHost()->maybeRedo())
  {
    if(ctrl->graph())
    {
      GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_nodePath.c_str());
      if(uiNode)
      {
        ctrl->moveNode(uiNode, m_pos, true);
        return true;
      }
    }
    else
      return true;
    return false;
  }
  return false;
}
