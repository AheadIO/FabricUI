// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGNotificationRouter.h"
#include "DFGGraph.h"
#include "DFGController.h"

#include <FTL/JSONValue.h>

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

void DFGNotificationRouter::onGraphSet()
{
  if(!m_coreDFGExec)
    return;

  FabricCore::DFGStringResult desc = m_coreDFGExec.getDesc();
  char const *descData;
  uint32_t descSize;
  desc.getStringDataAndLength( descData, descSize );
  FTL::JSONStrWithLoc jsonSrcWithLoc( FTL::StrRef( descData, descSize ) );
  FTL::OwnedPtr<FTL::JSONValue const> rootValue(
    FTL::JSONValue::Decode( jsonSrcWithLoc )
    );

  try
  {
    FTL::JSONObject const *rootObject = rootValue->cast<FTL::JSONObject>();

    FTL::JSONArray const *portsArray =
      rootObject->get( FTL_STR("ports") )->cast<FTL::JSONArray>();
    for ( size_t i = 0; i < portsArray->size(); ++i )
    {
      FTL::JSONObject const *portObject =
        portsArray->get( i )->cast<FTL::JSONObject>();
      onExecPortInserted( portObject );
    }

    FTL::JSONArray const *nodesArray =
      rootObject->get( FTL_STR("nodes") )->cast<FTL::JSONArray>();
    for ( size_t i = 0; i < nodesArray->size(); ++i )
    {
      FTL::JSONObject const *nodeObject =
        nodesArray->get( i )->cast<FTL::JSONObject>();
      onNodeInserted( nodeObject );
    }

  // DFGWrapper::ExecPortList ports = m_graph->getExecPorts();
  // for(size_t i=0;i<ports.size();i++)
  // {
  //   onExecPortInserted(ports[i]);
  // }

  // DFGWrapper::ConnectionList connections = m_graph->getConnections();
  // for(size_t i=0;i<connections.size();i++)
  // {
  //   onPortsConnected(connections[i]->getSrc(), connections[i]->getDst());
  // }

  // // update the graph's pan and zoom
  // std::string metaData = m_graph->getMetadata("uiGraphPan");
  // if(metaData.length())
  //   onExecMetadataChanged(m_graph, "uiGraphPan", metaData.c_str());
  // metaData = m_graph->getMetadata("uiGraphZoom");
  // if(metaData.length())
  //   onExecMetadataChanged(m_graph, "uiGraphZoom", metaData.c_str());
  }
  catch ( FTL::JSONException je )
  {
    printf( "Caught JSONException: %s\n", je.getDescCStr() );
  }
}

void DFGNotificationRouter::onNotification(FTL::CStrRef json)
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

void DFGNotificationRouter::onNodeInserted(FTL::JSONObject const *jsonObject)
{
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  if(!graph)
    return;

  FTL::CStrRef nodeName =
    jsonObject->get( FTL_STR("name") )->cast<FTL::JSONString>()->getValue();

  GraphView::Node * uiNode = graph->addNodeFromPreset( nodeName, FTL::CStrRef() );
  if(!uiNode)
    return;

  FTL::CStrRef title;
  if ( jsonObject->maybeGetString( FTL_STR("title"), title ) )
    onNodeTitleChanged( nodeName, title );

  FTL::JSONArray const *portsJSONArray =
    jsonObject->get( FTL_STR("ports") )->cast<FTL::JSONArray>();
  for ( size_t i = 0; i < portsJSONArray->size(); ++i )
  {
    FTL::JSONObject const *portJSONObject =
      portsJSONArray->get( i )->cast<FTL::JSONObject>();
    onNodePortInserted( nodeName, portJSONObject );
  }

  if ( FTL::JSONValue const *metadataJSONValue =
    jsonObject->maybeGet( FTL_STR("metadata") ) )
  {
    FTL::JSONObject const *metadataJSONObject =
      metadataJSONValue->cast<FTL::JSONObject>();

    for ( FTL::JSONObject::const_iterator it = metadataJSONObject->begin();
      it != metadataJSONObject->end(); ++it )
    {
      FTL::CStrRef key = it->first;
      FTL::CStrRef value = it->second->cast<FTL::JSONString>()->getValue();
      onNodeMetadataChanged(nodeName, key, value);
    }
  }

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }
}

void DFGNotificationRouter::onNodeRemoved(FabricCore::DFGExec parent, FTL::CStrRef nodePath)
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

void DFGNotificationRouter::onNodePortInserted(
  FTL::CStrRef nodeName,
  FTL::JSONObject const *jsonObject
  )
{
  DFGGraph * graph = (DFGGraph*)m_controller->graph();
  if(!graph)
    return;
  GraphView::Node * uiNode = graph->node(nodeName);
  if(!uiNode)
    return;

  FTL::CStrRef portName = jsonObject->getString( FTL_STR("name") );

  FTL::CStrRef dataType = jsonObject->getStringOrEmpty( FTL_STR("type") );

  FabricCore::DFGExec exec = getCoreDFGExec();
  QColor color = m_config.getColorForDataType(dataType, &exec, portName.c_str());

  FTL::CStrRef nodePortType =
    jsonObject->getStringOrEmpty( FTL_STR("nodePortType") );
  GraphView::PortType pType = GraphView::PortType_Input;
  if(nodePortType == FTL_STR("Out"))
    pType = GraphView::PortType_Output;
  else if(nodePortType == FTL_STR("IO"))
    pType = GraphView::PortType_IO;

  GraphView::Pin * uiPin = new GraphView::Pin(uiNode, portName.c_str(), pType, color, portName.c_str());
  if ( !dataType.empty() )
    uiPin->setDataType(dataType.c_str());
  uiNode->addPin(uiPin, false);
}

void DFGNotificationRouter::onNodePortRemoved(FabricCore::DFGExec parent, FTL::CStrRef nodePortPath)
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

void DFGNotificationRouter::onExecPortInserted(
  FTL::JSONObject const *jsonObject
  )
{
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  if(!uiGraph)
    return;

  FTL::CStrRef portName = jsonObject->getString( FTL_STR("name") );

  FTL::CStrRef dataType = jsonObject->getStringOrEmpty( FTL_STR("type") );

  FabricCore::DFGExec exec = getCoreDFGExec();
  QColor color = m_config.getColorForDataType(dataType, &exec, portName.c_str());

  GraphView::Port * uiOutPort = NULL;
  GraphView::Port * uiInPort = NULL;

  FTL::CStrRef execPortType =
    jsonObject->getStringOrEmpty( FTL_STR("execPortType") );
  if(execPortType != FTL_STR("In"))
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(!uiPanel)
      return;

    uiInPort = new GraphView::Port(
      uiPanel, portName, GraphView::PortType_Input, dataType, color, portName
      );
    uiPanel->addPort(uiInPort);
    m_lastPortInserted = uiInPort;
  }
  if(execPortType != FTL_STR("Out"))
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(!uiPanel)
      return;

    uiOutPort = new GraphView::Port(
      uiPanel, portName, GraphView::PortType_Output, dataType, color, portName
      );
    uiPanel->addPort(uiOutPort);
    m_lastPortInserted = uiOutPort;
  }
  if(uiOutPort && uiInPort)
  {
    uiGraph->addConnection(uiOutPort, uiInPort, false);
  }
}

void DFGNotificationRouter::onExecPortRemoved(FabricCore::DFGExec exec, FTL::CStrRef portPath)
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

void DFGNotificationRouter::onPortsConnected(FabricCore::DFGExec exec, FTL::CStrRef srcPath, FTL::CStrRef dstPath)
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

void DFGNotificationRouter::onPortsDisconnected(FabricCore::DFGExec exec, FTL::CStrRef srcPath, FTL::CStrRef dstPath)
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

void DFGNotificationRouter::onNodeMetadataChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef key,
  FTL::CStrRef metadata
  )
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  GraphView::Node * uiNode = uiGraph->node(nodeName.data());
  if(!uiNode)
    return;

  // printf("'%s' metadata changed for '%s'\n", key, path.toUtf8().constData());

  if(key == FTL_STR("uiGraphPos"))
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata.data());
    const FabricCore::Variant * xVar = metadataVar.getDictValue("x");
    const FabricCore::Variant * yVar = metadataVar.getDictValue("y");
    float x = getFloatFromVariant(xVar);
    float y = getFloatFromVariant(yVar);
    uiNode->setTopLeftGraphPos(QPointF(x, y), false);
  }
  else if(key == FTL_STR("uiCollapsedState"))
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(metadata.data());
    GraphView::Node::CollapseState state = (GraphView::Node::CollapseState)metadataVar.getSInt32();
    uiNode->setCollapsedState(state);
  }
  else if(key == FTL_STR("uiNodeColor"))
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
  else if(key == FTL_STR("uiHeaderColor"))
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
  else if(key == FTL_STR("uiTooltip"))
  {
    QString tooltip = metadata.data();
    uiNode->header()->setToolTip(tooltip.trimmed());
  }
}

void DFGNotificationRouter::onNodeTitleChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef title
  )
{
  if(m_controller->graph() == NULL)
    return;
  DFGGraph * uiGraph = (DFGGraph*)m_controller->graph();
  GraphView::Node * uiNode = uiGraph->node(nodeName);
  if(!uiNode)
    return;
  uiNode->setTitle(title);
  uiNode->update();
}


void DFGNotificationRouter::onExecPortRenamed(FabricCore::DFGExec exec, FTL::CStrRef oldPath, FTL::CStrRef newPath)
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

void DFGNotificationRouter::onNodePortRenamed(FabricCore::DFGExec parent, FTL::CStrRef oldPath, FTL::CStrRef newPath)
{
  // this shouldn't happen for us for now
}

void DFGNotificationRouter::onExecMetadataChanged(FabricCore::DFGExec exec, FTL::CStrRef key, FTL::CStrRef metadata)
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

void DFGNotificationRouter::onExtDepAdded(FabricCore::DFGExec exec, FTL::CStrRef extension, FTL::CStrRef version)
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

void DFGNotificationRouter::onExtDepRemoved(FabricCore::DFGExec exec, FTL::CStrRef extension, FTL::CStrRef version)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodeCacheRuleChanged(FabricCore::DFGExec parent, FTL::CStrRef path, FTL::CStrRef rule)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecCacheRuleChanged(FabricCore::DFGExec exec, FTL::CStrRef rule)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecPortResolvedTypeChanged(FabricCore::DFGExec exec,  FTL::CStrRef portPath, FTL::CStrRef resolvedType)
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

void DFGNotificationRouter::onExecPortTypeSpecChanged(FabricCore::DFGExec exec,  FTL::CStrRef portPath, FTL::CStrRef typeSpec)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortResolvedTypeChanged(FabricCore::DFGExec exec,  FTL::CStrRef nodePortPath, FTL::CStrRef resolvedType)
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

void DFGNotificationRouter::onExecPortMetadataChanged(
  FTL::CStrRef portPath,
  FTL::CStrRef key,
  FTL::CStrRef metadata)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortMetadataChanged(
  FTL::CStrRef nodePortPath,
  FTL::CStrRef key,
  FTL::CStrRef metadata)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecPortTypeChanged(
  FTL::CStrRef portPath,
  FTL::CStrRef portType
  )
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortTypeChanged(
  FTL::CStrRef nodePortPath,
  FTL::CStrRef portType
  )
{
  // todo: we don't do anything here...
}
