// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGRemoveConnectionCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRemoveConnectionCommand::DFGRemoveConnectionCommand(DFGController * controller, const char * srcPath, const char * dstPath, bool srcAsPin, bool dstAsPin)
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

bool DFGRemoveConnectionCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  DFGView * view = ctrl->getView();
  DFGWrapper::GraphExecutablePtr graph = view->getGraph();

  if(m_srcIsPin && m_dstIsPin)
  {
    DFGWrapper::NodePtr srcNode = ctrl->getNodeFromPath(m_srcNodePath.c_str());
    DFGWrapper::NodePtr dstNode = ctrl->getNodeFromPath(m_dstNodePath.c_str());
    DFGWrapper::NodePortPtr srcPin = srcNode->getNodePort(m_srcPinTitle.c_str());
    DFGWrapper::NodePortPtr dstPin = dstNode->getNodePort(m_dstPinTitle.c_str());
    srcPin->disconnectFrom(dstPin);
  }
  else if(!m_srcIsPin && !m_dstIsPin)
  {
    DFGWrapper::ExecutablePtr srcExec = ctrl->getExecFromPath(m_srcNodePath.c_str());
    DFGWrapper::ExecutablePtr dstExec = ctrl->getExecFromPath(m_dstNodePath.c_str());
    DFGWrapper::ExecPortPtr srcPort = srcExec->getExecPort(m_srcPinTitle.c_str());
    DFGWrapper::ExecPortPtr dstPort = dstExec->getExecPort(m_dstPinTitle.c_str());
    srcPort->disconnectFrom(dstPort);
  }
  else if(m_srcIsPin && !m_dstIsPin)
  {
    DFGWrapper::NodePtr srcNode = ctrl->getNodeFromPath(m_srcNodePath.c_str());
    DFGWrapper::ExecutablePtr dstExec = ctrl->getExecFromPath(m_dstNodePath.c_str());
    DFGWrapper::NodePortPtr srcPin = srcNode->getNodePort(m_srcPinTitle.c_str());
    DFGWrapper::ExecPortPtr dstPort = dstExec->getExecPort(m_dstPinTitle.c_str());
    srcPin->disconnectFrom(dstPort);
  }
  else if(!m_srcIsPin && m_dstIsPin)
  {
    DFGWrapper::ExecutablePtr srcExec = ctrl->getExecFromPath(m_srcNodePath.c_str());
    DFGWrapper::NodePtr dstNode = ctrl->getNodeFromPath(m_dstNodePath.c_str());
    DFGWrapper::ExecPortPtr srcPort = srcExec->getExecPort(m_srcPinTitle.c_str());
    DFGWrapper::NodePortPtr dstPin = dstNode->getNodePort(m_dstPinTitle.c_str());
    srcPort->disconnectFrom(dstPin);
  }
  return true;
}

bool DFGRemoveConnectionCommand::undo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeUndo();  
}

bool DFGRemoveConnectionCommand::redo()
{
  DFGController * ctrl = (DFGController*)controller();
  return ctrl->getHost()->maybeRedo();  
}

const char * DFGRemoveConnectionCommand::getSrcPath() const
{
  return m_srcPath.c_str();
}

const char * DFGRemoveConnectionCommand::getDstPath() const
{
  return m_dstPath.c_str();
}

bool DFGRemoveConnectionCommand::srcIsPin() const
{
  return m_srcIsPin;
}

bool DFGRemoveConnectionCommand::dstIsPin() const
{
  return m_dstIsPin;
}
