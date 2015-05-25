// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddNodeCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddNodeCommand::DFGAddNodeCommand(DFGController * controller, QString path, QString preset, QPointF pos)
: DFGCommand(controller)
{
  m_path = path.toUtf8().constData();
  m_preset = preset.toUtf8().constData();
  m_pos = pos;
}

std::string DFGAddNodeCommand::getPath() const
{
  return GraphView::parentPathSTL(m_nodePath);
}

std::string DFGAddNodeCommand::getNodePath() const
{
  return m_nodePath;
}

std::string DFGAddNodeCommand::getPreset() const
{
  return m_preset;
}

QPointF DFGAddNodeCommand::getPos() const
{
  return m_pos;
}

GraphView::Node * DFGAddNodeCommand::getNode()
{
  DFGController * ctrl = (DFGController*)controller();
  if(ctrl->graph())
    return ctrl->graph()->nodeFromPath(m_nodePath.c_str());
  return NULL;
}

bool DFGAddNodeCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGWrapper::GraphExecutablePtr graph = ctrl->getGraphExec();
  DFGWrapper::NodePtr node = graph->addNodeFromPreset(m_preset.c_str());
  m_nodePath = node->getName();
  ctrl->moveNode(m_nodePath.c_str(), m_pos, false);
  return true;
}

bool DFGAddNodeCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  if(ctrl->graph())
  {
    GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_nodePath.c_str());
    if(uiNode)
    {
      m_pos = uiNode->topLeftGraphPos();
      if(ctrl->getHost()->maybeUndo())
      {
        return true;
      }
    }
  }
  else
    return ctrl->getHost()->maybeUndo();

  return false;
}

bool DFGAddNodeCommand::redo()
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
      return false;
    }
    else
    {
      ctrl->moveNode(m_nodePath.c_str(), m_pos, false);
    }
    return true;
  }
  return false;
}
