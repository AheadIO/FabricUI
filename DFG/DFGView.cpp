// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGView.h"
#include "DFGGraph.h"
#include "DFGController.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGView::DFGView(DFGWrapper::GraphExecutable graph, const DFGConfig & config)
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

  std::vector<DFGWrapper::Node> nodes = m_graph.getNodes();
  for(size_t i=0;i<nodes.size();i++)
  {
    onNodeInserted(nodes[i]);

    // all pins
    std::vector<DFGWrapper::Pin> pins = nodes[i].getPins();
    for(size_t j=0;j<pins.size();j++)
    {
      onPinInserted(pins[j]);
    }
  }

  std::vector<DFGWrapper::Port> ports = m_graph.getPorts();
  for(size_t i=0;i<ports.size();i++)
  {
    onPortInserted(ports[i]);
  }

  std::vector<DFGWrapper::Connection> connections = m_graph.getConnections();
  for(size_t i=0;i<connections.size();i++)
  {
    onEndPointsConnected(connections[i].getSrc(), connections[i].getDst());
  }

  // update the graph's pan and zoom
  std::string metaData = m_graph.getMetadata("uiGraphPan");
  if(metaData.length())
    onExecMetadataChanged(m_graph, "uiGraphPan", metaData.c_str());
  metaData = m_graph.getMetadata("uiGraphZoom");
  if(metaData.length())
    onExecMetadataChanged(m_graph, "uiGraphZoom", metaData.c_str());

  m_performChecks = true;
}

void DFGView::onNotification(char const * json)
{
  // printf("%s\n", json);
}

void DFGView::onNodeInserted(DFGWrapper::Node node)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  GraphView::Node * uiNode = graph->addNodeFromPreset(node.getPath().c_str(), node.getTitle().c_str());
  if(!uiNode)
    return;
  std::string uiGraphPosMetadata = node.getMetadata("uiGraphPos");
  if(uiGraphPosMetadata.length() > 0)
  {
    onNodeMetadataChanged(node, "uiGraphPos", uiGraphPosMetadata.c_str());
  }

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onNodeRemoved(DFGWrapper::Node node)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  QString path = node.getPath().c_str();
  path = GraphView::relativePath(graph->path(), path).toUtf8().constData();
  GraphView::Node * uiNode = graph->node(path);
  if(!uiNode)
    return;
  graph->removeNode(uiNode);

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onPinInserted(DFGWrapper::Pin pin)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  QString path = GraphView::parentPath(pin.getPath().c_str());
  path = GraphView::relativePath(graph->path(), path).toUtf8().constData();;
  GraphView::Node * uiNode = graph->node(path);
  if(!uiNode)
    return;
  std::string dataType = pin.getDataType();
  if(dataType.length() == 0)
    dataType = DFGWrapper::Port(pin).getDataType();
  std::string name = pin.getName();
  QColor color = m_config.getColorForDataType(dataType);
  GraphView::PortType pType = GraphView::PortType_Input;
  if(pin.getPinType() == FabricCore::DFGPortType_Out)
    pType = GraphView::PortType_Output;
  else if(pin.getPinType() == FabricCore::DFGPortType_IO)
    pType = GraphView::PortType_IO;

  GraphView::Pin * uiPin = new GraphView::Pin(uiNode, name.c_str(), pType, color);
  uiPin->setDataType(dataType.c_str());
  uiNode->addPin(uiPin, false);
}

void DFGView::onPinRemoved(DFGWrapper::Pin pin)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  QString path = GraphView::parentPath(pin.getPath().c_str());
  path = GraphView::relativePath(graph->path(), path).toUtf8().constData();;
  GraphView::Node * uiNode = graph->node(path);
  if(!uiNode)
    return;
  GraphView::Pin * uiPin = uiNode->pin(pin.getName().c_str());
  if(!uiPin)
    return;
  uiNode->removePin(uiPin, false);

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onPortInserted(DFGWrapper::Port port)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  std::string dataType = port.getDataType();
  std::string path = port.getPath();
  std::string name = GraphView::relativePathSTL(m_graph.getPath(), path);

  QColor color = m_config.getColorForDataType(dataType);

  GraphView::Port * uiOutPort = NULL;
  GraphView::Port * uiInPort = NULL;

  if(port.getPortType() != FabricCore::DFGPortType_In)
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(!uiPanel)
      return;

    uiInPort = new GraphView::Port(uiPanel, name.c_str(), GraphView::PortType_Input, dataType.c_str(), color);
    uiPanel->addPort(uiInPort);
    m_lastPortInserted = uiInPort;
  }
  if(port.getPortType() != FabricCore::DFGPortType_Out)
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

void DFGView::onPortRemoved(DFGWrapper::Port port)
{
  if(m_controller->graph() == NULL)
    return;
  QString path = GraphView::parentPath(port.getPath().c_str());
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  GraphView::SidePanel * uiOutPanel = uiGraph->sidePanel(GraphView::PortType_Output);
  if(!uiOutPanel)
    return;
  GraphView::SidePanel * uiInPanel = uiGraph->sidePanel(GraphView::PortType_Input);
  if(!uiInPanel)
    return;
  GraphView::Port * uiOutPort = uiOutPanel->port(port.getName().c_str());
  GraphView::Port * uiInPort = uiInPanel->port(port.getName().c_str());

  if(port.getPortType() == FabricCore::DFGPortType_IO && uiOutPort && uiInPort)
  {
    uiGraph->removeConnection(uiOutPort, uiInPort);
  }
  if(port.getPortType() != FabricCore::DFGPortType_In)
  {
    if(!uiInPort)
      return;
    uiInPanel->removePort(uiInPort);
  }
  if(port.getPortType() != FabricCore::DFGPortType_Out)
  {
    if(!uiOutPort)
      return;
    uiOutPanel->removePort(uiOutPort);
  }

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onEndPointsConnected(DFGWrapper::Port src, DFGWrapper::Port dst)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  DFGWrapper::GraphExecutable graph = m_controller->getView()->getGraph();

  QString srcParentPath = GraphView::parentPath(src.getPath().c_str()).toUtf8().constData();
  QString dstParentPath = GraphView::parentPath(dst.getPath().c_str()).toUtf8().constData();
  srcParentPath = GraphView::relativePath(uiGraph->path(), srcParentPath).toUtf8().constData();
  dstParentPath = GraphView::relativePath(uiGraph->path(), dstParentPath).toUtf8().constData();

  GraphView::ConnectionTarget * uiSrcTarget = NULL;
  GraphView::ConnectionTarget * uiDstTarget = NULL;

  if(srcParentPath.length() > 0)
  {
    GraphView::Node * uiSrcNode = uiGraph->node(srcParentPath);
    if(!uiSrcNode)
      return;
    uiSrcTarget = uiSrcNode->pin(src.getName().c_str());
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(uiPanel)
      uiSrcTarget = uiPanel->port(src.getName().c_str());
  }

  if(dstParentPath.length() > 0)
  {
    GraphView::Node * uiDstNode = uiGraph->node(dstParentPath);
    if(!uiDstNode)
      return;
    uiDstTarget = uiDstNode->pin(dst.getName().c_str());
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(uiPanel)
      uiDstTarget = uiPanel->port(dst.getName().c_str());
  }

  if(!uiSrcTarget || !uiDstTarget)
    return;

  uiGraph->addConnection(uiSrcTarget, uiDstTarget, false);

  if(m_performChecks)
    updateDataTypesOnPorts();

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onEndPointsDisconnected(DFGWrapper::Port src, DFGWrapper::Port dst)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * graph = (DFGGraph*)m_controller->graph();

  QString srcParentPath = GraphView::parentPath(src.getPath().c_str()).toUtf8().constData();
  QString dstParentPath = GraphView::parentPath(dst.getPath().c_str()).toUtf8().constData();
  srcParentPath = GraphView::relativePath(graph->path(), srcParentPath).toUtf8().constData();
  dstParentPath = GraphView::relativePath(graph->path(), dstParentPath).toUtf8().constData();

  GraphView::ConnectionTarget * uiSrcTarget = NULL;
  GraphView::ConnectionTarget * uiDstTarget = NULL;

  if(srcParentPath.length() > 0)
  {
    GraphView::Node * uiSrcNode = graph->node(srcParentPath);
    if(!uiSrcNode)
      return;
    uiSrcTarget = uiSrcNode->pin(src.getName().c_str());
  }
  else
  {
    GraphView::SidePanel * uiPanel = graph->sidePanel(GraphView::PortType_Output);
    if(uiPanel)
      uiSrcTarget = uiPanel->port(src.getName().c_str());
  }

  if(dstParentPath.length() > 0)
  {
    GraphView::Node * uiDstNode = graph->node(dstParentPath);
    if(!uiDstNode)
      return;
    uiDstTarget = uiDstNode->pin(dst.getName().c_str());
  }
  else
  {
    GraphView::SidePanel * uiPanel = graph->sidePanel(GraphView::PortType_Input);
    if(uiPanel)
      uiDstTarget = uiPanel->port(dst.getName().c_str());
  }

  if(!uiSrcTarget || !uiDstTarget)
    return;

  graph->removeConnection(uiSrcTarget, uiDstTarget, false);

  if(m_performChecks)
    updateDataTypesOnPorts();

  if(m_performChecks)
    m_controller->checkErrors();
}

void DFGView::onNodeMetadataChanged(DFGWrapper::Node node, const char * key, const char * metadata)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  QString path = GraphView::relativePath(uiGraph->path(), node.getPath().c_str());
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

void DFGView::onNodeTitleChanged(DFGWrapper::Node node, const char * title)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  QString path = GraphView::relativePath(uiGraph->path(), node.getPath().c_str());
  GraphView::Node * uiNode = uiGraph->node(path);
  if(!uiNode)
    return;
  uiNode->setTitle(title);
  uiNode->update();
}

void DFGView::onPortRenamed(DFGWrapper::Port port, const char * oldName)
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();

  GraphView::SidePanel * uiPanel = uiGraph->sidePanel(port.getPortType() == FabricCore::DFGPortType_Out ? GraphView::PortType_Input : GraphView::PortType_Output);
  if(!uiPanel)
    return;

  GraphView::Port * uiPort = uiPanel->port(oldName);
  if(!uiPort)
    return;
  uiPort->setName(port.getName().c_str());
}

void DFGView::onPinRenamed(DFGWrapper::Pin pin, const char * oldName)
{
  // this shouldn't happen for us for now
}

void DFGView::onExecMetadataChanged(DFGWrapper::Executable exec, const char * key, const char * metadata)
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

void DFGView::onNodeCacheRuleChanged(const char * path, const char * rule)
{
  // todo: we don't do anything here...
}

void DFGView::onExecCacheRuleChanged(const char * path, const char * rule)
{
  // todo: we don't do anything here...
}

void DFGView::updateDataTypesOnPorts()
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  DFGWrapper::GraphExecutable graph = m_controller->getView()->getGraph();

  // todo: this should really not be necessary. ideally we would receive notifications
  // from the core for any type change.
  std::vector<GraphView::Node*> uiNodes = uiGraph->nodes();
  for(size_t i=0;i<uiNodes.size();i++)
  {
    GraphView::Node * uiNode = uiNodes[i];
    DFGWrapper::Node node = graph.getNode(uiNode->name().toUtf8().constData());

    // pull on getdesc to update the node's description.
    try
    {
      std::string string = node.getDesc();
      // printf( "getDesc %s\n", string.c_str() );
    }
    catch(FabricCore::Exception e)
    {
      continue;
    }

    std::vector<DFGWrapper::Pin> pins = node.getPins();
    for(size_t j=0;j<pins.size();j++)
    {
      GraphView::Pin * uiPin = uiNode->pin(pins[j].getName().c_str());
      if(!uiPin)
        continue;
      std::string dataType = pins[j].getDataType();
      if(dataType != uiPin->dataType().toUtf8().constData())
      {
        uiPin->setDataType(dataType.c_str());
        uiPin->setColor(m_config.getColorForDataType(dataType), false);
      }
    }    
  }

  std::vector<GraphView::Port*> uiPorts = uiGraph->ports();
  for(size_t i=0;i<uiPorts.size();i++)
  {
    GraphView::Port * uiPort = uiPorts[i];
    DFGWrapper::Port port = graph.getPort(uiPort->name().toUtf8().constData());

    // pull on getdesc to update the node's description.
    try
    {
      graph.getDesc();
      std::string dataType = port.getDataType();
      if(dataType != uiPort->dataType().toUtf8().constData())
      {
        uiPort->setDataType(dataType.c_str());
        uiPort->setColor(m_config.getColorForDataType(dataType));
      }
    }
    catch(FabricCore::Exception e)
    {
      continue;
    }
  }

}
