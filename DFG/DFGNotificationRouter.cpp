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

/*
void DFGNotificationRouter::onNodeRemoved(DFGWrapper::NodePtr node)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  QString path = node->getNodeName();
  GraphView::Node * uiNode = graph->nodeFromPath(path);
  if(!uiNode)
    return;
  graph->removeNode(uiNode);

  if(m_performChecks)
    m_controller->checkErrors();
}
*/

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
  uiPin->setDataType(dataType.data());
  uiNode->addPin(uiPin, false);
}

/*
void DFGNotificationRouter::onNodePortRemoved(DFGWrapper::NodePortPtr pin)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  QString path = GraphView::parentPath(pin->getPortPath());
  QString name = GraphView::lastPathSegment(pin->getPortPath());
  GraphView::Node * uiNode = graph->nodeFromPath(path);
  if(!uiNode)
    return;
  GraphView::Pin * uiPin = uiNode->pin(name);
  if(!uiPin)
    return;
  uiNode->removePin(uiPin, false);

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGNotificationRouter::onExecPortInserted(DFGWrapper::ExecPortPtr port)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  std::string dataType = port->getResolvedType();
  if(dataType.empty())
    dataType = port->getTypeSpec();
  std::string path = port->getPortPath();
  std::string name = port->getPortName();

  // todo: once titles are supports
  // std::string label = port->getTitle();
  std::string label = port->getPortName();

  QColor color = m_config.getColorForDataType(dataType, port);

  GraphView::Port * uiOutPort = NULL;
  GraphView::Port * uiInPort = NULL;

  if(port->getNodePortType() != FabricCore::DFGPortType_Out)
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(!uiPanel)
      return;

    uiInPort = new GraphView::Port(uiPanel, name.c_str(), GraphView::PortType_Input, dataType.c_str(), color, label.c_str());
    uiPanel->addPort(uiInPort);
    m_lastPortInserted = uiInPort;
  }
  if(port->getNodePortType() != FabricCore::DFGPortType_In)
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(!uiPanel)
      return;

    uiOutPort = new GraphView::Port(uiPanel, name.c_str(), GraphView::PortType_Output, dataType.c_str(), color, label.c_str());
    uiPanel->addPort(uiOutPort);
    m_lastPortInserted = uiOutPort;
  }
  if(uiOutPort && uiInPort)
  {
    uiGraph->addConnection(uiOutPort, uiInPort, false);
  }
}

void DFGNotificationRouter::onExecPortRemoved(DFGWrapper::ExecPortPtr port)
{
  if(m_controller->graph() == NULL)
    return;
  QString path = GraphView::parentPath(port->getPortPath());
  QString name = GraphView::lastPathSegment(port->getPortPath());
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  GraphView::SidePanel * uiOutPanel = uiGraph->sidePanel(GraphView::PortType_Output);
  if(!uiOutPanel)
    return;
  GraphView::SidePanel * uiInPanel = uiGraph->sidePanel(GraphView::PortType_Input);
  if(!uiInPanel)
    return;
  GraphView::Port * uiOutPort = uiOutPanel->port(name);
  GraphView::Port * uiInPort = uiInPanel->port(name);

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
    m_controller->checkErrors();
}

void DFGNotificationRouter::onPortsConnected(DFGWrapper::PortPtr src, DFGWrapper::PortPtr dst)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  DFGWrapper::GraphExecutablePtr graph = m_controller->getGraph();

  QString srcParentPath = GraphView::parentPath(src->getPortPath()).toUtf8().constData();
  QString dstParentPath = GraphView::parentPath(dst->getPortPath()).toUtf8().constData();

  GraphView::ConnectionTarget * uiSrcTarget = NULL;
  GraphView::ConnectionTarget * uiDstTarget = NULL;

  if(srcParentPath.length() > 0)
  {
    GraphView::Node * uiSrcNode = uiGraph->nodeFromPath(srcParentPath);
    if(!uiSrcNode)
      return;
    uiSrcTarget = uiSrcNode->pin(src->getPortName());
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(uiPanel)
      uiSrcTarget = uiPanel->port(src->getPortName());
  }

  if(dstParentPath.length() > 0)
  {
    GraphView::Node * uiDstNode = uiGraph->nodeFromPath(dstParentPath);
    if(!uiDstNode)
      return;
    uiDstTarget = uiDstNode->pin(dst->getPortName());
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(uiPanel)
      uiDstTarget = uiPanel->port(dst->getPortName());
  }

  if(!uiSrcTarget || !uiDstTarget)
    return;

  uiGraph->addConnection(uiSrcTarget, uiDstTarget, false);

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGNotificationRouter::onPortsDisconnected(DFGWrapper::PortPtr src, DFGWrapper::PortPtr dst)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();

  QString srcParentPath = GraphView::parentPath(src->getPortPath()).toUtf8().constData();
  QString dstParentPath = GraphView::parentPath(dst->getPortPath()).toUtf8().constData();
  QString srcName = GraphView::lastPathSegment(src->getPortPath());
  QString dstName = GraphView::lastPathSegment(dst->getPortPath());

  GraphView::ConnectionTarget * uiSrcTarget = NULL;
  GraphView::ConnectionTarget * uiDstTarget = NULL;

  if(srcParentPath.length() > 0)
  {
    GraphView::Node * uiSrcNode = graph->nodeFromPath(srcParentPath);
    if(!uiSrcNode)
      return;
    uiSrcTarget = uiSrcNode->pin(srcName);
  }
  else
  {
    GraphView::SidePanel * uiPanel = graph->sidePanel(GraphView::PortType_Output);
    if(uiPanel)
      uiSrcTarget = uiPanel->port(srcName);
  }

  if(dstParentPath.length() > 0)
  {
    GraphView::Node * uiDstNode = graph->nodeFromPath(dstParentPath);
    if(!uiDstNode)
      return;
    uiDstTarget = uiDstNode->pin(dstName);
  }
  else
  {
    GraphView::SidePanel * uiPanel = graph->sidePanel(GraphView::PortType_Input);
    if(uiPanel)
      uiDstTarget = uiPanel->port(dstName);
  }

  if(!uiSrcTarget || !uiDstTarget)
    return;

  graph->removeConnection(uiSrcTarget, uiDstTarget, false);

  if(m_performChecks)
    m_controller->checkErrors();
}
*/

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

/*
void DFGNotificationRouter::onExecPortRenamed(DFGWrapper::ExecPortPtr port, const char * oldName)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  GraphView::SidePanel * uiPanel = uiGraph->sidePanel(port->getNodePortType() == FabricCore::DFGPortType_In ? GraphView::PortType_Input : GraphView::PortType_Output);
  if(!uiPanel)
    return;

  GraphView::Port * uiPort = uiPanel->port(oldName);
  if(!uiPort)
    return;
  uiPort->setName(port->getPortName());
}

void DFGNotificationRouter::onNodePortRenamed(DFGWrapper::NodePortPtr pin, const char * oldName)
{
  // this shouldn't happen for us for now
}

void DFGNotificationRouter::onExecMetadataChanged(DFGWrapper::ExecutablePtr exec, const char * key, const char * metadata)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  // printf("'%s' metadata changed for '%s'\n", key, exec->getExecPath());

  if(key == std::string("uiGraphPan"))
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata);
    const FabricCore::Variant * xVar = metadataVar.getDictValue("x");
    const FabricCore::Variant * yVar = metadataVar.getDictValue("y");
    float x = getFloatFromVariant(xVar);
    float y = getFloatFromVariant(yVar);
    uiGraph->mainPanel()->setCanvasPan(QPointF(x, y), false);
  }
  else if(key == std::string("uiGraphZoom"))
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata);
    const FabricCore::Variant * valueVar = metadataVar.getDictValue("value");
    const FabricCore::Variant * yVar = metadataVar.getDictValue("y");
    float value = getFloatFromVariant(valueVar);
    uiGraph->mainPanel()->setCanvasZoom(value, false);
  }
}

void DFGNotificationRouter::onExtDepAdded(const char * extension, const char * version)
{
  if(m_controller->graph() == NULL)
    return;
  try
  {
    m_controller->getClient()->loadExtension(extension, version, false);
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }
}

void DFGNotificationRouter::onExtDepRemoved(const char * extension, const char * version)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodeCacheRuleChanged(const char * path, const char * rule)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecCacheRuleChanged(const char * path, const char * rule)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecPortResolvedTypeChanged(DFGWrapper::ExecPortPtr port, const char * resolvedType)
{
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  if(!uiGraph)
    return;
  GraphView::Port * uiPort = uiGraph->port(port->getPortName());
  if(!uiPort)
    return;

  if(resolvedType != uiPort->dataType())
  {
    uiPort->setDataType(resolvedType);
    uiPort->setColor(m_config.getColorForDataType(resolvedType, port));
    uiGraph->updateColorForConnections(uiPort);
  }
}

void DFGNotificationRouter::onExecPortTypeSpecChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * typeSpec)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortResolvedTypeChanged(DFGWrapper::NodePortPtr nodePort, const char * resolvedType)
{
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  if(!uiGraph)
    return;
  DFGWrapper::NodePtr node = nodePort->getNode();
  GraphView::Node * uiNode = uiGraph->nodeFromPath(node->getNodeName());
  if(!uiNode)
    return;
  GraphView::Pin * uiPin = uiNode->pin(nodePort->getPortName());
  if(!uiPin)
    return;

  if(resolvedType != uiPin->dataType())
  {
    uiPin->setDataType(resolvedType);
    if ( nodePort->isInstPort() )
    {
      DFGWrapper::InstPortPtr instPotr =
        DFGWrapper::InstPortPtr::StaticCast( nodePort );
      uiPin->setColor(m_config.getColorForDataType(resolvedType, instPotr->getExecPort()), false);
    }
    uiGraph->updateColorForConnections(uiPin);
  }
}

void DFGNotificationRouter::onExecPortMetadataChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * key, const char * metadata)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortMetadataChanged(FabricServices::DFGWrapper::NodePortPtr pin, const char * key, const char * metadata)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortTypeChanged(FabricServices::DFGWrapper::NodePortPtr pin, FabricCore::DFGPortType pinType)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecPortTypeChanged(FabricServices::DFGWrapper::ExecPortPtr port, FabricCore::DFGPortType portType)
{
  // todo: we don't do anything here...
}
*/
