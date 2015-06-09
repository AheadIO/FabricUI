// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGNotificationRouter.h"
#include "DFGGraph.h"
#include "DFGController.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGNotificationRouter::DFGNotificationRouter(
  FabricCore::DFGBinding coreDFGBinding,
  FabricCore::DFGExec coreDFGGraph,
  const DFGConfig & config
  )
  : NotificationRouter( coreDFGBinding, coreDFGGraph )
{
  m_config = config;
  m_lastPortInserted = NULL;
  m_performChecks = true;
}

GraphView::Port * DFGNotificationRouter::getLastPortInserted()
{
  return m_lastPortInserted;
}

float DFGNotificationRouter::getFloatFromVariant(const FabricCore::Variant * variant)
{
  if(variant == 0)
    return 0.0f;
  if(variant->isSInt8())
    return variant->getSInt8();
  else if(variant->isSInt16())
    return variant->getSInt16();
  else if(variant->isSInt32())
    return variant->getSInt32();
  else if(variant->isSInt64())
    return variant->getSInt64();
  else if(variant->isFloat32())
    return variant->getFloat32();
  else if(variant->isFloat64())
    return variant->getFloat64();
  return 0.0f;
}

/*
void DFGNotificationRouter::onGraphSet()
{
  if(m_controller->graph() == NULL)
    return;

  m_performChecks = false;

  DFGWrapper::NodeList nodes = m_graph->getNodes();
  for(size_t i=0;i<nodes.size();i++)
  {
    onNodeInserted(nodes[i]);

    // all pins
    DFGWrapper::NodePortList pins = nodes[i]->getNodePorts();
    for(size_t j=0;j<pins.size();j++)
    {
      onNodePortInserted(pins[j]);
    }
  }

  DFGWrapper::ExecPortList ports = m_graph->getExecPorts();
  for(size_t i=0;i<ports.size();i++)
  {
    onExecPortInserted(ports[i]);
  }

  DFGWrapper::ConnectionList connections = m_graph->getConnections();
  for(size_t i=0;i<connections.size();i++)
  {
    onPortsConnected(connections[i]->getSrc(), connections[i]->getDst());
  }

  // update the graph's pan and zoom
  std::string metaData = m_graph->getMetadata("uiGraphPan");
  if(metaData.length())
    onExecMetadataChanged(m_graph, "uiGraphPan", metaData.c_str());
  metaData = m_graph->getMetadata("uiGraphZoom");
  if(metaData.length())
    onExecMetadataChanged(m_graph, "uiGraphZoom", metaData.c_str());

  m_performChecks = true;
}

*/

void DFGNotificationRouter::onNotification(FTL::StrRef json)
{
  // // todo: remove this
  // FabricCore::Variant notificationVar = FabricCore::Variant::CreateFromJSON(json);
  // const FabricCore::Variant * descVar = notificationVar.getDictValue("desc");
  // std::string descStr = descVar->getStringData();
  // if(descStr != "nodeMetadataChanged" && descStr != "execMetadataChanged")
  // {
  //   m_controller->log(("View::callback "+std::string(json)).c_str());    
  // }
}

void DFGNotificationRouter::onNodeInserted(FabricCore::DFGExec parent, FTL::StrRef nodePath)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();

  FabricCore::DFGExec exec = parent.getSubExec(nodePath.data());

  GraphView::Node * uiNode = graph->addNodeFromPreset(nodePath.data(), exec.getTitle());
  if(!uiNode)
    return;

  FTL::StrRef uiGraphPosMetadata = parent.getNodeMetadata(nodePath.data(), "uiGraphPos");
  if(uiGraphPosMetadata.size() > 0)
    onNodeMetadataChanged(parent, nodePath, "uiGraphPos", uiGraphPosMetadata);

  std::string uiCollapsedStateMetadata = parent.getNodeMetadata(nodePath.data(), "uiCollapsedState");
  if(uiCollapsedStateMetadata.size() > 0)
    onNodeMetadataChanged(parent, nodePath, "uiCollapsedState", uiCollapsedStateMetadata);

  std::string uiNodeColorMetadata = parent.getNodeMetadata(nodePath.data(), "uiNodeColor");
  if(uiNodeColorMetadata.size() == 0 && exec.isValid())
    uiNodeColorMetadata = exec.getMetadata("uiNodeColor");
  if(uiNodeColorMetadata.size() > 0)
    onNodeMetadataChanged(parent, nodePath, "uiNodeColor", uiNodeColorMetadata);

  std::string uiHeaderColorMetadata = parent.getNodeMetadata(nodePath.data(), "uiHeaderColor");
  if(uiHeaderColorMetadata.size() == 0 && exec.isValid())
    uiHeaderColorMetadata = exec.getMetadata("uiHeaderColor");
  if(uiHeaderColorMetadata.size() > 0)
    onNodeMetadataChanged(parent, nodePath, "uiHeaderColor", uiHeaderColorMetadata);

  std::string uiTooltipMetadata = parent.getNodeMetadata(nodePath.data(), "uiTooltip");
  if(uiTooltipMetadata.size() == 0 && exec.isValid())
    uiTooltipMetadata = exec.getMetadata("uiTooltip");
  if(uiTooltipMetadata.size() > 0)
    onNodeMetadataChanged(parent, nodePath, "uiTooltip", uiTooltipMetadata);

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }
}

void DFGNotificationRouter::onNodeRemoved(FabricCore::DFGExec parent, FTL::StrRef nodePath)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  GraphView::Node * uiNode = graph->node(nodePath.data());
  if(!uiNode)
    return;
  graph->removeNode(uiNode);

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }
}

void DFGNotificationRouter::onNodePortInserted(FabricCore::DFGExec parent, FTL::StrRef nodePortPath)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();

  int delimPos = nodePortPath.find('.') - nodePortPath.data();
  FTL::StrRef nodeName = nodePortPath.substr(0, delimPos);
  FTL::StrRef portName = nodePortPath.substr(delimPos+1);

  // the first part of the substr won't 
  // work since there is no \0 char. 
  // casting it to a std::string does
  std::string nodeNameStr(nodeName);

  GraphView::Node * uiNode = graph->node(nodeNameStr.c_str());
  if(!uiNode)
    return;

  FabricCore::DFGExec subExec = parent.getSubExec(nodeNameStr.c_str());
  FTL::StrRef dataType = parent.getNodePortResolvedType(nodePortPath.data());
  QColor color = m_config.getColorForDataType(dataType, &subExec, portName.data());
  GraphView::PortType pType = GraphView::PortType_Input;
  if(subExec.getExecPortType(portName.data()) == FabricCore::DFGPortType_Out)
    pType = GraphView::PortType_Output;
  else if(subExec.getExecPortType(portName.data()) == FabricCore::DFGPortType_IO)
    pType = GraphView::PortType_IO;

  GraphView::Pin * uiPin = new GraphView::Pin(uiNode, portName.data(), pType, color, portName.data());
  if ( !dataType.empty() )
    uiPin->setDataType(dataType.data());
  uiNode->addPin(uiPin, false);
}

void DFGNotificationRouter::onNodePortRemoved(FabricCore::DFGExec parent, FTL::StrRef nodePortPath)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();

  int delimPos = nodePortPath.find('.') - nodePortPath.data();
  FTL::StrRef nodeName = nodePortPath.substr(0, delimPos);
  FTL::StrRef portName = nodePortPath.substr(delimPos+1);

  // the first part of the substr won't 
  // work since there is no \0 char. 
  // casting it to a std::string does
  std::string nodeNameStr(nodeName);

  GraphView::Node * uiNode = graph->nodeFromPath(nodeNameStr.c_str());
  if(!uiNode)
    return;
  GraphView::Pin * uiPin = uiNode->pin(portName.data());
  if(!uiPin)
    return;
  uiNode->removePin(uiPin, false);

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }
}

void DFGNotificationRouter::onExecPortInserted(FabricCore::DFGExec exec, FTL::StrRef portPath)
{
  if(m_controller->graph() == NULL)
    return;

  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  FTL::StrRef dataType = exec.getExecPortResolvedType(portPath.data());
  if(dataType.empty())
    dataType = exec.getExecPortTypeSpec(portPath.data());

  QColor color = m_config.getColorForDataType(dataType, &exec, portPath.data());

  GraphView::Port * uiOutPort = NULL;
  GraphView::Port * uiInPort = NULL;

  if(exec.getExecPortType(portPath.data()) != FabricCore::DFGPortType_Out)
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(!uiPanel)
      return;

    uiInPort = new GraphView::Port(uiPanel, portPath.data(), GraphView::PortType_Input, dataType.data(), color, portPath.data());
    uiPanel->addPort(uiInPort);
    m_lastPortInserted = uiInPort;
  }
  if(exec.getExecPortType(portPath.data()) != FabricCore::DFGPortType_In)
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(!uiPanel)
      return;

    uiOutPort = new GraphView::Port(uiPanel, portPath.data(), GraphView::PortType_Input, dataType.data(), color, portPath.data());
    uiPanel->addPort(uiOutPort);
    m_lastPortInserted = uiOutPort;
  }
  if(uiOutPort && uiInPort)
  {
    uiGraph->addConnection(uiOutPort, uiInPort, false);
  }
}

void DFGNotificationRouter::onExecPortRemoved(FabricCore::DFGExec exec, FTL::StrRef portPath)
{
  if(m_controller->graph() == NULL)
    return;
 
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  GraphView::SidePanel * uiOutPanel = uiGraph->sidePanel(GraphView::PortType_Output);
  if(!uiOutPanel)
    return;
  GraphView::SidePanel * uiInPanel = uiGraph->sidePanel(GraphView::PortType_Input);
  if(!uiInPanel)
    return;
  GraphView::Port * uiOutPort = uiOutPanel->port(portPath.data());
  GraphView::Port * uiInPort = uiInPanel->port(portPath.data());

  if(uiOutPort && uiInPort)
  {
    uiGraph->removeConnection(uiOutPort, uiInPort);
  }
  if(uiInPort)
  {
    uiInPanel->removePort(uiInPort);
  }
  if(uiOutPort)
  {
    uiOutPanel->removePort(uiOutPort);
  }

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }
}

void DFGNotificationRouter::onPortsConnected(FabricCore::DFGExec exec, FTL::StrRef srcPath, FTL::StrRef dstPath)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  GraphView::ConnectionTarget * uiSrcTarget = NULL;
  GraphView::ConnectionTarget * uiDstTarget = NULL;

  if(srcPath.contains('.'))
  {
    int delimPos = srcPath.find('.') - srcPath.data();
    FTL::StrRef nodeName = srcPath.substr(0, delimPos);
    FTL::StrRef portName = srcPath.substr(delimPos+1);
    std::string nodeNameStr(nodeName);

    GraphView::Node * uiSrcNode = uiGraph->node(nodeNameStr.c_str());
    if(!uiSrcNode)
      return;
    uiSrcTarget = uiSrcNode->pin(portName.data());
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(uiPanel)
      uiSrcTarget = uiPanel->port(srcPath.data());
  }

  if(dstPath.contains('.'))
  {
    int delimPos = dstPath.find('.') - dstPath.data();
    FTL::StrRef nodeName = dstPath.substr(0, delimPos);
    FTL::StrRef portName = dstPath.substr(delimPos+1);
    std::string nodeNameStr(nodeName);

    GraphView::Node * uiDstNode = uiGraph->node(nodeNameStr.c_str());
    if(!uiDstNode)
      return;
    uiDstTarget = uiDstNode->pin(portName.data());
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(uiPanel)
      uiDstTarget = uiPanel->port(dstPath.data());
  }

  if(!uiSrcTarget || !uiDstTarget)
    return;

  uiGraph->addConnection(uiSrcTarget, uiDstTarget, false);

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }
}

void DFGNotificationRouter::onPortsDisconnected(FabricCore::DFGExec exec, FTL::StrRef srcPath, FTL::StrRef dstPath)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  GraphView::ConnectionTarget * uiSrcTarget = NULL;
  GraphView::ConnectionTarget * uiDstTarget = NULL;

  if(srcPath.contains('.'))
  {
    int delimPos = srcPath.find('.') - srcPath.data();
    FTL::StrRef nodeName = srcPath.substr(0, delimPos);
    FTL::StrRef portName = srcPath.substr(delimPos+1);
    std::string nodeNameStr(nodeName);

    GraphView::Node * uiSrcNode = uiGraph->node(nodeNameStr.c_str());
    if(!uiSrcNode)
      return;
    uiSrcTarget = uiSrcNode->pin(portName.data());
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(uiPanel)
      uiSrcTarget = uiPanel->port(srcPath.data());
  }

  if(dstPath.contains('.'))
  {
    int delimPos = dstPath.find('.') - dstPath.data();
    FTL::StrRef nodeName = dstPath.substr(0, delimPos);
    FTL::StrRef portName = dstPath.substr(delimPos+1);
    std::string nodeNameStr(nodeName);

    GraphView::Node * uiDstNode = uiGraph->node(nodeNameStr.c_str());
    if(!uiDstNode)
      return;
    uiDstTarget = uiDstNode->pin(portName.data());
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(uiPanel)
      uiDstTarget = uiPanel->port(dstPath.data());
  }

  if(!uiSrcTarget || !uiDstTarget)
    return;

  uiGraph->removeConnection(uiSrcTarget, uiDstTarget, false);

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }
}

void DFGNotificationRouter::onNodeMetadataChanged(FabricCore::DFGExec parent, FTL::StrRef nodePath, FTL::StrRef key, FTL::StrRef metadata)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  GraphView::Node * uiNode = uiGraph->node(nodePath.data());
  if(!uiNode)
    return;

  // printf("'%s' metadata changed for '%s'\n", key, path.toUtf8().constData());

  if(key == "uiGraphPos")
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata.data());
    const FabricCore::Variant * xVar = metadataVar.getDictValue("x");
    const FabricCore::Variant * yVar = metadataVar.getDictValue("y");
    float x = getFloatFromVariant(xVar);
    float y = getFloatFromVariant(yVar);
    uiNode->setTopLeftGraphPos(QPointF(x, y), false);
  }
  else if(key == "uiCollapsedState")
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata.data());
    GraphView::Node::CollapseState state = (GraphView::Node::CollapseState)metadataVar.getSInt32();
    uiNode->setCollapsedState(state);
  }
  else if(key == "uiNodeColor")
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata.data());
    const FabricCore::Variant * rVar = metadataVar.getDictValue("r");
    const FabricCore::Variant * gVar = metadataVar.getDictValue("g");
    const FabricCore::Variant * bVar = metadataVar.getDictValue("b");
    int r = (int)getFloatFromVariant(rVar);
    int g = (int)getFloatFromVariant(gVar);
    int b = (int)getFloatFromVariant(bVar);

    QColor color(r, g, b);
    uiNode->setColor(color);
    uiNode->setLabelColor(color.darker(130));
  }
  else if(key == "uiHeaderColor")
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata.data());
    const FabricCore::Variant * rVar = metadataVar.getDictValue("r");
    const FabricCore::Variant * gVar = metadataVar.getDictValue("g");
    const FabricCore::Variant * bVar = metadataVar.getDictValue("b");
    int r = (int)getFloatFromVariant(rVar);
    int g = (int)getFloatFromVariant(gVar);
    int b = (int)getFloatFromVariant(bVar);

    QColor color(r, g, b);
    uiNode->setLabelColor(color);
  }
  else if(key == "uiTooltip")
  {
    QString tooltip = metadata.data();
    uiNode->header()->setToolTip(tooltip.trimmed());
  }
}

void DFGNotificationRouter::onNodeTitleChanged(FabricCore::DFGExec parent, FTL::StrRef nodePath, FTL::StrRef title)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  GraphView::Node * uiNode = uiGraph->node(nodePath.data());
  if(!uiNode)
    return;
  uiNode->setTitle(title.data());
  uiNode->update();
}


void DFGNotificationRouter::onExecPortRenamed(FabricCore::DFGExec exec, FTL::StrRef oldPath, FTL::StrRef newPath)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  FabricCore::DFGPortType portType = exec.getExecPortType(newPath.data());
  GraphView::SidePanel * uiPanel = uiGraph->sidePanel(portType == FabricCore::DFGPortType_In ? GraphView::PortType_Input : GraphView::PortType_Output);
  if(!uiPanel)
    return;

  GraphView::Port * uiPort = uiPanel->port(oldPath.data());
  if(!uiPort)
    return;
  uiPort->setName(newPath.data());
}

void DFGNotificationRouter::onNodePortRenamed(FabricCore::DFGExec parent, FTL::StrRef oldPath, FTL::StrRef newPath)
{
  // this shouldn't happen for us for now
}

void DFGNotificationRouter::onExecMetadataChanged(FabricCore::DFGExec exec, FTL::StrRef key, FTL::StrRef metadata)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  // printf("'%s' metadata changed for '%s'\n", key, exec->getExecPath());

  if(key == "uiGraphPan")
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata.data());
    const FabricCore::Variant * xVar = metadataVar.getDictValue("x");
    const FabricCore::Variant * yVar = metadataVar.getDictValue("y");
    float x = getFloatFromVariant(xVar);
    float y = getFloatFromVariant(yVar);
    uiGraph->mainPanel()->setCanvasPan(QPointF(x, y), false);
  }
  else if(key == "uiGraphZoom")
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata.data());
    const FabricCore::Variant * valueVar = metadataVar.getDictValue("value");
    const FabricCore::Variant * yVar = metadataVar.getDictValue("y");
    float value = getFloatFromVariant(valueVar);
    uiGraph->mainPanel()->setCanvasZoom(value, false);
  }
}

void DFGNotificationRouter::onExtDepAdded(FabricCore::DFGExec exec, FTL::StrRef extension, FTL::StrRef version)
{
  if(m_controller->graph() == NULL)
    return;
  try
  {
    FabricCore::Client client = ((DFGController*)m_controller)->getClient();
    client.loadExtension(extension.data(), version.data(), false);
  }
  catch(FabricCore::Exception e)
  {
    ((DFGController*)m_controller)->logError(e.getDesc_cstr());
  }
}

void DFGNotificationRouter::onExtDepRemoved(FabricCore::DFGExec exec, FTL::StrRef extension, FTL::StrRef version)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodeCacheRuleChanged(FabricCore::DFGExec parent, FTL::StrRef path, FTL::StrRef rule)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecCacheRuleChanged(FabricCore::DFGExec exec, FTL::StrRef rule)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecPortResolvedTypeChanged(FabricCore::DFGExec exec,  FTL::StrRef portPath, FTL::StrRef resolvedType)
{
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  if(!uiGraph)
    return;
  GraphView::Port * uiPort = uiGraph->port(portPath.data());
  if(!uiPort)
    return;

  if(resolvedType != uiPort->dataType())
  {
    uiPort->setDataType(resolvedType.data());
    uiPort->setColor(m_config.getColorForDataType(resolvedType, &exec, portPath.data()));
    uiGraph->updateColorForConnections(uiPort);
  }
}

void DFGNotificationRouter::onExecPortTypeSpecChanged(FabricCore::DFGExec exec,  FTL::StrRef portPath, FTL::StrRef typeSpec)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortResolvedTypeChanged(FabricCore::DFGExec exec,  FTL::StrRef nodePortPath, FTL::StrRef resolvedType)
{
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  if(!uiGraph)
    return;

  int delimPos = nodePortPath.find('.') - nodePortPath.data();
  FTL::StrRef nodeName = nodePortPath.substr(0, delimPos);
  FTL::StrRef portName = nodePortPath.substr(delimPos+1);

  // the first part of the substr won't 
  // work since there is no \0 char. 
  // casting it to a std::string does
  std::string nodeNameStr(nodeName);
 
  GraphView::Node * uiNode = uiGraph->node(nodeNameStr.c_str());
  if(!uiNode)
    return;
  GraphView::Pin * uiPin = uiNode->pin(portName.data());
  if(!uiPin)
    return;

  if(resolvedType != uiPin->dataType())
  {
    FabricCore::DFGExec subExec = exec.getSubExec(nodeNameStr.c_str());
    uiPin->setDataType(resolvedType.data());
    uiPin->setColor(m_config.getColorForDataType(resolvedType, &subExec, portName.data()));
    uiGraph->updateColorForConnections(uiPin);
  }
}

void DFGNotificationRouter::onExecPortMetadataChanged(FabricCore::DFGExec exec, FTL::StrRef portPath, FTL::StrRef key, FTL::StrRef metadata)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortMetadataChanged(FabricCore::DFGExec parent, FTL::StrRef nodePortPath, FTL::StrRef key, FTL::StrRef metadata)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecPortTypeChanged(FabricCore::DFGExec exec, FTL::StrRef portPath, FTL::StrRef portType)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortTypeChanged(FabricCore::DFGExec parent, FTL::StrRef nodePortPath, FTL::StrRef portType)
{
  // todo: we don't do anything here...
}
