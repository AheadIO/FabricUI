// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddEmptyFuncCommand.h"
#include <DFGWrapper/Inst.h>

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

std::string DFGAddEmptyFuncCommand::getInstPath() const
{
  return m_instPath;
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
  DFGWrapper::GraphExecutablePtr graph = ctrl->getGraphExec();
  DFGWrapper::InstPtr inst = graph->addInstWithNewFunc(m_title.c_str());
  inst->getExecutable()->setTitle(m_title.c_str());
  m_instPath = inst->getNodeName();
  if(ctrl->graph())
  {
    GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_instPath.c_str());
    if(uiNode)
      ctrl->moveNode(uiNode, m_pos, false);
  }
  return true;
}

bool DFGAddEmptyFuncCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_instPath.c_str());
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
      GraphView::Node * uiNode = ctrl->graph()->nodeFromPath(m_instPath.c_str());
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
