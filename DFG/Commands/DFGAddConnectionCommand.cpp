// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddConnectionCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddConnectionCommand::DFGAddConnectionCommand(DFGController * controller, const char * srcPath, const char * dstPath, bool srcAsPin, bool dstAsPin)
: DFGCommand(controller)
{
  m_srcIsPin = srcAsPin;
  m_dstIsPin = dstAsPin;
  m_srcPath = srcPath;
  m_dstPath = dstPath;
  m_srcPinTitle = GraphView::lastPathSegmentSTL(m_srcPath);
  m_dstPinTitle = GraphView::lastPathSegmentSTL(m_dstPath);
  m_srcNodePath = GraphView::parentPathSTL(m_srcPath);
  m_dstNodePath = GraphView::parentPathSTL(m_dstPath);
}

bool DFGAddConnectionCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGView * view = ctrl->getView();
  DFGWrapper::GraphExecutablePtr graph = view->getGraph();

  if(m_srcIsPin && m_dstIsPin)
  {
    DFGWrapper::NodePtr srcNode = ctrl->getNodeFromPath(m_srcNodePath.c_str());
    DFGWrapper::NodePtr dstNode = ctrl->getNodeFromPath(m_dstNodePath.c_str());
    DFGWrapper::NodePortPtr srcPin = srcNode->getPort(m_srcPinTitle.c_str());
    DFGWrapper::NodePortPtr dstPin = dstNode->getPort(m_dstPinTitle.c_str());
    srcPin->connectTo(dstPin);
  }
  else if(!m_srcIsPin && !m_dstIsPin)
  {
    DFGWrapper::ExecutablePtr srcExec = ctrl->getExecFromPath(m_srcNodePath.c_str());
    DFGWrapper::ExecutablePtr dstExec = ctrl->getExecFromPath(m_dstNodePath.c_str());
    DFGWrapper::ExecPortPtr srcPort = srcExec->getPort(m_srcPinTitle.c_str());
    DFGWrapper::ExecPortPtr dstPort = dstExec->getPort(m_dstPinTitle.c_str());
    srcPort->connectTo(dstPort);
  }
  else if(m_srcIsPin && !m_dstIsPin)
  {
    DFGWrapper::NodePtr srcNode = ctrl->getNodeFromPath(m_srcNodePath.c_str());
    DFGWrapper::ExecutablePtr dstExec = ctrl->getExecFromPath(m_dstNodePath.c_str());
    DFGWrapper::NodePortPtr srcPin = srcNode->getPort(m_srcPinTitle.c_str());
    DFGWrapper::ExecPortPtr dstPort = dstExec->getPort(m_dstPinTitle.c_str());
    srcPin->connectTo(dstPort);
  }
  else if(!m_srcIsPin && m_dstIsPin)
  {
    DFGWrapper::ExecutablePtr srcExec = ctrl->getExecFromPath(m_srcNodePath.c_str());
    DFGWrapper::NodePtr dstNode = ctrl->getNodeFromPath(m_dstNodePath.c_str());
    DFGWrapper::ExecPortPtr srcPort = srcExec->getPort(m_srcPinTitle.c_str());
    DFGWrapper::NodePortPtr dstPin = dstNode->getPort(m_dstPinTitle.c_str());
    srcPort->connectTo(dstPin);
  }
  return true;
}

bool DFGAddConnectionCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();  
}

bool DFGAddConnectionCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();  
}

const char * DFGAddConnectionCommand::getSrcPath() const
{
  return m_srcPath.c_str();
}

const char * DFGAddConnectionCommand::getDstPath() const
{
  return m_dstPath.c_str();
}

bool DFGAddConnectionCommand::srcIsPin() const
{
  return m_srcIsPin;
}

bool DFGAddConnectionCommand::dstIsPin() const
{
  return m_dstIsPin;
}
