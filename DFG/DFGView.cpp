// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGView.h"
#include "DFGGraph.h"
#include "DFGController.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGView::DFGView(DFGWrapper::GraphExecutablePtr graph, const DFGConfig & config)
: DFGWrapper::View(graph)
, m_graph(graph)
{
  m_controller = false;
  m_config = config;
  m_lastPortInserted = NULL;
  m_performChecks = true;
}

DFGController * DFGView::getController()
{
  return m_controller;
}

void DFGView::setController(DFGController * controller)
{
  m_controller = controller;
}

DFGWrapper::GraphExecutablePtr DFGView::getGraph()
{
  return m_graph;
}

GraphView::Port * DFGView::getLastPortInserted()
{
  return m_lastPortInserted;
}

float DFGView::getFloatFromVariant(const FabricCore::Variant * variant)
{
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

void DFGView::onGraphSet()
{
  if(m_controller->graph() == NULL)
    return;

  m_performChecks = false;

  DFGWrapper::NodeList nodes = m_graph->getNodes();
  for(size_t i=0;i<nodes.size();i++)
  {
    onNodeInserted(nodes[i]);

    // all pins
    DFGWrapper::PinList pins = nodes[i]->getPins();
    for(size_t j=0;j<pins.size();j++)
    {
      onPinInserted(pins[j]);
    }
  }

  DFGWrapper::PortList ports = m_graph->getPorts();
  for(size_t i=0;i<ports.size();i++)
  {
    onPortInserted(ports[i]);
  }

  DFGWrapper::ConnectionList connections = m_graph->getConnections();
  for(size_t i=0;i<connections.size();i++)
  {
    onEndPointsConnected(connections[i]->getSrc(), connections[i]->getDst());
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

void DFGView::onNotification(char const * json)
{
  // printf("%s\n", json);
}

void DFGView::onNodeInserted(DFGWrapper::NodePtr node)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  GraphView::Node * uiNode = graph->addNodeFromPreset(node->getNodePath(), node->getTitle());
  if(!uiNode)
    return;
  std::string uiGraphPosMetadata = node->getMetadata("uiGraphPos");
  if(uiGraphPosMetadata.length() > 0)
  {
    onNodeMetadataChanged(node, "uiGraphPos", uiGraphPosMetadata.c_str());
  }

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onNodeRemoved(DFGWrapper::NodePtr node)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  QString path = node->getNodePath();
  GraphView::Node * uiNode = graph->node(path);
  if(!uiNode)
    return;
  graph->removeNode(uiNode);

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onPinInserted(DFGWrapper::PinPtr pin)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  QString path = GraphView::parentPath(pin->getEndPointPath());
  GraphView::Node * uiNode = graph->node(path);
  if(!uiNode)
    return;
  std::string dataType = pin->getResolvedType();
  if(dataType.empty())
    dataType = pin->getPort()->getTypeSpec();
  std::string name = pin->getName();
  QColor color = m_config.getColorForDataType(dataType);
  GraphView::PortType pType = GraphView::PortType_Input;
  if(pin->getEndPointType() == FabricCore::DFGPortType_Out)
    pType = GraphView::PortType_Output;
  else if(pin->getEndPointType() == FabricCore::DFGPortType_IO)
    pType = GraphView::PortType_IO;

  GraphView::Pin * uiPin = new GraphView::Pin(uiNode, name.c_str(), pType, color);
  uiPin->setDataType(dataType.c_str());
  uiNode->addPin(uiPin, false);
}

void DFGView::onPinRemoved(DFGWrapper::PinPtr pin)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  QString path = GraphView::parentPath(pin->getEndPointPath());
  QString name = GraphView::lastPathSegment(pin->getEndPointPath());
  GraphView::Node * uiNode = graph->node(path);
  if(!uiNode)
    return;
  GraphView::Pin * uiPin = uiNode->pin(name);
  if(!uiPin)
    return;
  uiNode->removePin(uiPin, false);

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onPortInserted(DFGWrapper::PortPtr port)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  std::string dataType = port->getResolvedType();
  if(dataType.empty())
    dataType = port->getTypeSpec();
  std::string path = port->getEndPointPath();
  std::string name = port->getName();

  QColor color = m_config.getColorForDataType(dataType);

  GraphView::Port * uiOutPort = NULL;
  GraphView::Port * uiInPort = NULL;

  if(port->getEndPointType() != FabricCore::DFGPortType_Out)
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(!uiPanel)
      return;

    uiInPort = new GraphView::Port(uiPanel, name.c_str(), GraphView::PortType_Input, dataType.c_str(), color);
    uiPanel->addPort(uiInPort);
    m_lastPortInserted = uiInPort;
  }
  if(port->getEndPointType() != FabricCore::DFGPortType_In)
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(!uiPanel)
      return;

    uiOutPort = new GraphView::Port(uiPanel, name.c_str(), GraphView::PortType_Output, dataType.c_str(), color);
    uiPanel->addPort(uiOutPort);
    m_lastPortInserted = uiOutPort;
  }
  if(uiOutPort && uiInPort)
  {
    uiGraph->addConnection(uiOutPort, uiInPort, false);
  }
}

void DFGView::onPortRemoved(DFGWrapper::PortPtr port)
{
  if(m_controller->graph() == NULL)
    return;
  QString path = GraphView::parentPath(port->getEndPointPath());
  QString name = GraphView::lastPathSegment(port->getEndPointPath());
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

  if(port->getEndPointType() == FabricCore::DFGPortType_IO && uiOutPort && uiInPort)
  {
    uiGraph->removeConnection(uiOutPort, uiInPort);
  }
  if(port->getEndPointType() != FabricCore::DFGPortType_In)
  {
    if(!uiInPort)
      return;
    uiInPanel->removePort(uiInPort);
  }
  if(port->getEndPointType() != FabricCore::DFGPortType_Out)
  {
    if(!uiOutPort)
      return;
    uiOutPanel->removePort(uiOutPort);
  }

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onEndPointsConnected(DFGWrapper::EndPointPtr src, DFGWrapper::EndPointPtr dst)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  DFGWrapper::GraphExecutablePtr graph = m_controller->getView()->getGraph();

  QString srcParentPath = GraphView::parentPath(src->getEndPointPath()).toUtf8().constData();
  QString dstParentPath = GraphView::parentPath(dst->getEndPointPath()).toUtf8().constData();

  GraphView::ConnectionTarget * uiSrcTarget = NULL;
  GraphView::ConnectionTarget * uiDstTarget = NULL;

  if(srcParentPath.length() > 0)
  {
    GraphView::Node * uiSrcNode = uiGraph->node(srcParentPath);
    if(!uiSrcNode)
      return;
    uiSrcTarget = uiSrcNode->pin(src->getName());
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(uiPanel)
      uiSrcTarget = uiPanel->port(src->getName());
  }

  if(dstParentPath.length() > 0)
  {
    GraphView::Node * uiDstNode = uiGraph->node(dstParentPath);
    if(!uiDstNode)
      return;
    uiDstTarget = uiDstNode->pin(dst->getName());
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(uiPanel)
      uiDstTarget = uiPanel->port(dst->getName());
  }

  if(!uiSrcTarget || !uiDstTarget)
    return;

  uiGraph->addConnection(uiSrcTarget, uiDstTarget, false);

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onEndPointsDisconnected(DFGWrapper::EndPointPtr src, DFGWrapper::EndPointPtr dst)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();

  QString srcParentPath = GraphView::parentPath(src->getEndPointPath()).toUtf8().constData();
  QString dstParentPath = GraphView::parentPath(dst->getEndPointPath()).toUtf8().constData();

  GraphView::ConnectionTarget * uiSrcTarget = NULL;
  GraphView::ConnectionTarget * uiDstTarget = NULL;

  if(srcParentPath.length() > 0)
  {
    GraphView::Node * uiSrcNode = graph->node(srcParentPath);
    if(!uiSrcNode)
      return;
    uiSrcTarget = uiSrcNode->pin(src->getName());
  }
  else
  {
    GraphView::SidePanel * uiPanel = graph->sidePanel(GraphView::PortType_Output);
    if(uiPanel)
      uiSrcTarget = uiPanel->port(src->getName());
  }

  if(dstParentPath.length() > 0)
  {
    GraphView::Node * uiDstNode = graph->node(dstParentPath);
    if(!uiDstNode)
      return;
    uiDstTarget = uiDstNode->pin(dst->getName());
  }
  else
  {
    GraphView::SidePanel * uiPanel = graph->sidePanel(GraphView::PortType_Input);
    if(uiPanel)
      uiDstTarget = uiPanel->port(dst->getName());
  }

  if(!uiSrcTarget || !uiDstTarget)
    return;

  graph->removeConnection(uiSrcTarget, uiDstTarget, false);

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onNodeMetadataChanged(DFGWrapper::NodePtr node, const char * key, const char * metadata)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  QString path = node->getNodePath();
  GraphView::Node * uiNode = uiGraph->node(path);
  if(!uiNode)
    return;

  if(key == std::string("uiGraphPos"))
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata);
    const FabricCore::Variant * xVar = metadataVar.getDictValue("x");
    const FabricCore::Variant * yVar = metadataVar.getDictValue("y");
    float x = getFloatFromVariant(xVar);
    float y = getFloatFromVariant(yVar);
    uiNode->setTopLeftGraphPos(QPointF(x, y), false);
  }
}

void DFGView::onNodeTitleChanged(DFGWrapper::NodePtr node, const char * title)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  QString path = node->getNodePath();
  GraphView::Node * uiNode = uiGraph->node(path);
  if(!uiNode)
    return;
  uiNode->setTitle(title);
  uiNode->update();
}

void DFGView::onPortRenamed(DFGWrapper::PortPtr port, const char * oldName)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  GraphView::SidePanel * uiPanel = uiGraph->sidePanel(port->getEndPointType() == FabricCore::DFGPortType_Out ? GraphView::PortType_Input : GraphView::PortType_Output);
  if(!uiPanel)
    return;

  GraphView::Port * uiPort = uiPanel->port(oldName);
  if(!uiPort)
    return;
  uiPort->setName(port->getName());
}

void DFGView::onPinRenamed(DFGWrapper::PinPtr pin, const char * oldName)
{
  // this shouldn't happen for us for now
}

void DFGView::onExecMetadataChanged(DFGWrapper::ExecutablePtr exec, const char * key, const char * metadata)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

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

void DFGView::onExtDepAdded(const char * extension, const char * version)
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

void DFGView::onExtDepRemoved(const char * extension, const char * version)
{
  // todo: we don't do anything here...
}

void DFGView::onNodeCacheRuleChanged(const char * path, const char * rule)
{
  // todo: we don't do anything here...
}

void DFGView::onExecCacheRuleChanged(const char * path, const char * rule)
{
  // todo: we don't do anything here...
}

void DFGView::onPortResolvedTypeChanged(DFGWrapper::PortPtr port, const char * resolvedType)
{
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  if(!uiGraph)
    return;
  GraphView::Port * uiPort = uiGraph->port(port->getName());
  if(!uiPort)
    return;

  if(resolvedType != uiPort->dataType())
  {
    uiPort->setDataType(resolvedType);
    uiPort->setColor(m_config.getColorForDataType(resolvedType));
  }
}

void DFGView::onPinResolvedTypeChanged(DFGWrapper::PinPtr pin, const char * resolvedType)
{
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  if(!uiGraph)
    return;
  DFGWrapper::NodePtr node = pin->getNode();
  GraphView::Node * uiNode = uiGraph->node(node->getNodePath());
  if(!uiNode)
    return;
  GraphView::Pin * uiPin = uiNode->pin(pin->getName());
  if(!uiPin)
    return;

  if(resolvedType != uiPin->dataType())
  {
    uiPin->setDataType(resolvedType);
    uiPin->setColor(m_config.getColorForDataType(resolvedType), false);
  }
}

// void DFGView::updateDataTypesOnPorts()
// {
//   if(m_controller->graph() == NULL)
//     return;
//   DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
//   DFGWrapper::GraphExecutablePtr graph = m_controller->getView()->getGraph();

//   // todo: this should really not be necessary. ideally we would receive notifications
//   // from the core for any type change.
//   std::vector<GraphView::Node*> uiNodes = uiGraph->nodes();
//   for(size_t i=0;i<uiNodes.size();i++)
//   {
//     GraphView::Node * uiNode = uiNodes[i];
//     DFGWrapper::NodePtr node = graph->getNode(uiNode->name().toUtf8().constData());

//     // pull on getdesc to update the node's description.
//     try
//     {
//       // todo
//       // std::string string = node.getDesc();
//       // printf( "getDesc %s\n", string.c_str() );
//     }
//     catch(FabricCore::Exception e)
//     {
//       continue;
//     }

//     DFGWrapper::PinList pins = node->getPins();
//     for(size_t j=0;j<pins.size();j++)
//     {
//       GraphView::Pin * uiPin = uiNode->pin(pins[j]->getName());
//       if(!uiPin)
//         continue;
//       std::string dataType = pins[j]->getResolvedType();
//       if(dataType != uiPin->dataType().toUtf8().constData())
//       {
//         uiPin->setDataType(dataType.c_str());
//         uiPin->setColor(m_config.getColorForDataType(dataType), false);
//       }
//     }    
//   }

//   std::vector<GraphView::Port*> uiPorts = uiGraph->ports();
//   for(size_t i=0;i<uiPorts.size();i++)
//   {
//     GraphView::Port * uiPort = uiPorts[i];
//     DFGWrapper::PortPtr port = graph->getPort(uiPort->name().toUtf8().constData());

//     // pull on getdesc to update the node's description.
//     try
//     {
//       // todo
//       // graph.getDesc();
//       std::string dataType = port->getResolvedType();
//       if(dataType != uiPort->dataType().toUtf8().constData())
//       {
//         uiPort->setDataType(dataType.c_str());
//         uiPort->setColor(m_config.getColorForDataType(dataType));
//       }
//     }
//     catch(FabricCore::Exception e)
//     {
//       continue;
//     }
//   }

// }
