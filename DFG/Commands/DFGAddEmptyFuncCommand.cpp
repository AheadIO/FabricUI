// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddEmptyFuncCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddEmptyFuncCommand::DFGAddEmptyFuncCommand(DFGController * controller, QString path, QString title, QPointF pos)
: DFGCommand(controller)
{
  m_path = path.toUtf8().constData();
  m_title = title.toUtf8().constData();
  m_pos = pos;
}

std::string DFGAddEmptyFuncCommand::getPath() const
{
  return m_path;
}

std::string DFGAddEmptyFuncCommand::getTitle() const
{
  return m_title;
}

QPointF DFGAddEmptyFuncCommand::getPos() const
{
  return m_pos;
}

std::string DFGAddEmptyFuncCommand::getNodePath() const
{
  return m_nodePath;
}

GraphView::Node * DFGAddEmptyFuncCommand::getNode()
{
  DFGController * ctrl = (DFGController*)controller();
  if(!ctrl->graph())
    return NULL;
  return ctrl->graph()->nodeFromPath(m_nodePath.c_str());
}

bool DFGAddEmptyFuncCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGWrapper::GraphExecutablePtr graph = ctrl->getGraphExec();
  DFGWrapper::NodePtr node = graph->addNodeWithNewFunc(m_title.c_str());
  m_nodePath = node->getNodePath();
  if(ctrl->graph())
  {
    GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_nodePath.c_str());
    if(uiNode)
      ctrl->moveNode(uiNode, m_pos, false);
  }
  return true;
}

bool DFGAddEmptyFuncCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_nodePath.c_str());
  if(ctrl->getHost()->maybeUndo())
  {
    if(ctrl->graph())
    {
      if(uiNode)
      {
        m_pos = uiNode->topLeftGraphPos();
        return true;
      }
    }
    else
      return true;
  }
  return false;
}

bool DFGAddEmptyFuncCommand::redo()
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
  }
  return false;
}
