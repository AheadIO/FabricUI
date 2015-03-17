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
  DFGWrapper::GraphExecutable graph = view->getGraph();

  if(m_srcIsPin && m_dstIsPin)
  {
    DFGWrapper::Node srcNode = ctrl->getNodeFromPath(m_srcNodePath.c_str());
    DFGWrapper::Node dstNode = ctrl->getNodeFromPath(m_dstNodePath.c_str());
    DFGWrapper::Pin srcPin = srcNode.getPin(m_srcPinTitle.c_str());
    DFGWrapper::Pin dstPin = dstNode.getPin(m_dstPinTitle.c_str());
    srcPin.connect(dstPin);
  }
  else if(!m_srcIsPin && !m_dstIsPin)
  {
    DFGWrapper::Executable srcExec = ctrl->getExecFromPath(m_srcNodePath.c_str());
    DFGWrapper::Executable dstExec = ctrl->getExecFromPath(m_dstNodePath.c_str());
    DFGWrapper::Port srcPort = srcExec.getPort(m_srcPinTitle.c_str());
    DFGWrapper::Port dstPort = dstExec.getPort(m_dstPinTitle.c_str());
    srcPort.connect(dstPort);
  }
  else if(m_srcIsPin && !m_dstIsPin)
  {
    DFGWrapper::Node srcNode = ctrl->getNodeFromPath(m_srcNodePath.c_str());
    DFGWrapper::Executable dstExec = ctrl->getExecFromPath(m_dstNodePath.c_str());
    DFGWrapper::Pin srcPin = srcNode.getPin(m_srcPinTitle.c_str());
    DFGWrapper::Port dstPort = dstExec.getPort(m_dstPinTitle.c_str());
    srcPin.connect(dstPort);
  }
  else if(!m_srcIsPin && m_dstIsPin)
  {
    DFGWrapper::Executable srcExec = ctrl->getExecFromPath(m_srcNodePath.c_str());
    DFGWrapper::Node dstNode = ctrl->getNodeFromPath(m_dstNodePath.c_str());
    DFGWrapper::Port srcPort = srcExec.getPort(m_srcPinTitle.c_str());
    DFGWrapper::Pin dstPin = dstNode.getPin(m_dstPinTitle.c_str());
    srcPort.connect(dstPin);
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
