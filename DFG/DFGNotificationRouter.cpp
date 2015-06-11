// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/Graph.h>
#include "DFGNotificationRouter.h"
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
      onExecPortInserted(
        portObject->getString( FTL_STR("name") ),
        portObject
        );
    }

    FTL::JSONArray const *nodesArray =
      rootObject->get( FTL_STR("nodes") )->cast<FTL::JSONArray>();
    for ( size_t i = 0; i < nodesArray->size(); ++i )
    {
      FTL::JSONObject const *nodeObject =
        nodesArray->get( i )->cast<FTL::JSONObject>();
      onNodeInserted(
        nodeObject->getString( FTL_STR("name") ),
        nodeObject
        );
    }

    FTL::JSONObject const *connectionsObject =
      rootObject->get( FTL_STR("connections") )->cast<FTL::JSONObject>();
    for ( FTL::JSONObject::const_iterator it = connectionsObject->begin();
      it != connectionsObject->end(); ++it )
    {
      FTL::CStrRef srcPath = it->first;
      FTL::JSONArray const *dstsArray = it->second->cast<FTL::JSONArray>();
      for ( FTL::JSONArray::const_iterator it = dstsArray->begin();
        it != dstsArray->end(); ++it )
      {
        FTL::JSONValue const *dstValue = *it;
        FTL::CStrRef dstPath = dstValue->getStringValue();
        onPortsConnected( srcPath, dstPath );
      }
    }

    FTL::JSONValue const *metadatasValue =
      rootObject->maybeGet( FTL_STR("metadata") );
    if ( metadatasValue )
    {
      FTL::JSONObject const *metadatasObject =
        metadatasValue->cast<FTL::JSONObject>();
      for ( FTL::JSONObject::const_iterator it = metadatasObject->begin();
        it != metadatasObject->end(); ++it )
      {
        FTL::CStrRef key = it->first;
        FTL::CStrRef value = it->second->getStringValue();
        onExecMetadataChanged( key, value );
      }
    }
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

void DFGNotificationRouter::onNodeInserted(
  FTL::CStrRef nodeName,
  FTL::JSONObject const *jsonObject
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;

  GraphView::Node * uiNode =
    uiGraph->addNodeFromPreset( nodeName, FTL::CStrRef() );
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
    onNodePortInserted(
      nodeName,
      portJSONObject->getString( FTL_STR("name") ),
      portJSONObject
      );
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

void DFGNotificationRouter::onNodeRemoved(
  FTL::CStrRef nodeName
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;
  GraphView::Node * uiNode = uiGraph->node(nodeName);
  if(!uiNode)
    return;
  uiGraph->removeNode(uiNode);

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }
}

void DFGNotificationRouter::onNodePortInserted(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::JSONObject const *jsonObject
  )
{
  GraphView::Graph * graph = m_controller->graph();
  if(!graph)
    return;
  GraphView::Node * uiNode = graph->node(nodeName);
  if(!uiNode)
    return;

  FTL::CStrRef dataType = jsonObject->getStringOrEmpty( FTL_STR("type") );

  FabricCore::DFGExec exec = getCoreDFGExec();
  FabricCore::DFGExec subExec = exec.getSubExec(nodeName.c_str());
  QColor color = m_config.getColorForDataType(dataType, &subExec, portName.c_str());

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

void DFGNotificationRouter::onNodePortRemoved(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;

  GraphView::Node * uiNode = uiGraph->node(nodeName);
  if(!uiNode)
    return;
  GraphView::Pin * uiPin = uiNode->pin(portName);
  if(!uiPin)
    return;
  uiNode->removePin(uiPin, false);

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }
}

void DFGNotificationRouter::onExecPortInserted(
  FTL::CStrRef portName,
  FTL::JSONObject const *jsonObject
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;

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

void DFGNotificationRouter::onExecPortRemoved(
  FTL::CStrRef portName
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;

  GraphView::SidePanel * uiOutPanel = uiGraph->sidePanel(GraphView::PortType_Output);
  if(!uiOutPanel)
    return;
  GraphView::SidePanel * uiInPanel = uiGraph->sidePanel(GraphView::PortType_Input);
  if(!uiInPanel)
    return;
  GraphView::Port * uiOutPort = uiOutPanel->port(portName);
  GraphView::Port * uiInPort = uiInPanel->port(portName);

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

void DFGNotificationRouter::onPortsConnected(
  FTL::CStrRef srcPath,
  FTL::CStrRef dstPath
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;

  GraphView::ConnectionTarget * uiSrcTarget = NULL;
  GraphView::ConnectionTarget * uiDstTarget = NULL;

  std::pair<FTL::StrRef, FTL::CStrRef> srcSplit = srcPath.split('.');
  if(!srcSplit.second.empty())
  {
    GraphView::Node * uiSrcNode = uiGraph->node(srcSplit.first);
    if(!uiSrcNode)
      return;
    uiSrcTarget = uiSrcNode->pin(srcSplit.second);
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(uiPanel)
      uiSrcTarget = uiPanel->port(srcSplit.first);
  }

  std::pair<FTL::StrRef, FTL::CStrRef> dstSplit = dstPath.split('.');
  if(!dstSplit.second.empty())
  {
    GraphView::Node * uiDstNode = uiGraph->node(dstSplit.first);
    if(!uiDstNode)
      return;
    uiDstTarget = uiDstNode->pin(dstSplit.second);
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(uiPanel)
      uiDstTarget = uiPanel->port(dstSplit.first);
  }

  if(!uiSrcTarget || !uiDstTarget)
    return;

  uiGraph->addConnection(uiSrcTarget, uiDstTarget, false);

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }
}

void DFGNotificationRouter::onPortsDisconnected(
  FTL::CStrRef srcPath,
  FTL::CStrRef dstPath
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;

  GraphView::ConnectionTarget * uiSrcTarget = NULL;
  GraphView::ConnectionTarget * uiDstTarget = NULL;

  std::pair<FTL::StrRef, FTL::CStrRef> srcSplit = srcPath.split('.');
  if(!srcSplit.second.empty())
  {
    GraphView::Node * uiSrcNode = uiGraph->node(srcSplit.first);
    if(!uiSrcNode)
      return;
    uiSrcTarget = uiSrcNode->pin(srcSplit.second);
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Output);
    if(uiPanel)
      uiSrcTarget = uiPanel->port(srcSplit.first);
  }

  std::pair<FTL::StrRef, FTL::CStrRef> dstSplit = dstPath.split('.');
  if(!dstSplit.second.empty())
  {
    GraphView::Node * uiDstNode = uiGraph->node(dstSplit.first);
    if(!uiDstNode)
      return;
    uiDstTarget = uiDstNode->pin(dstSplit.second);
  }
  else
  {
    GraphView::SidePanel * uiPanel = uiGraph->sidePanel(GraphView::PortType_Input);
    if(uiPanel)
      uiDstTarget = uiPanel->port(dstSplit.first);
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
  FTL::CStrRef value
  )
{
  if(m_controller->graph() == NULL)
    return;
  GraphView::Graph * uiGraph = m_controller->graph();
  GraphView::Node * uiNode = uiGraph->node(nodeName.data());
  if(!uiNode)
    return;

  // printf("'%s' metadata changed for '%s'\n", key, path.toUtf8().constData());

  if(key == FTL_STR("uiGraphPos"))
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( value );
    FTL::OwnedPtr<FTL::JSONValue const> jsonValue(
      FTL::JSONValue::Decode( jsonStrWithLoc )
      );
    if ( jsonValue )
    {
      FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();
      float x = jsonObject->getFloat64( FTL_STR("x") );
      float y = jsonObject->getFloat64( FTL_STR("y") );
      uiNode->setTopLeftGraphPos(QPointF(x, y), false);
    }
  }
  else if(key == FTL_STR("uiCollapsedState"))
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(value.c_str());
    GraphView::Node::CollapseState state = (GraphView::Node::CollapseState)metadataVar.getSInt32();
    uiNode->setCollapsedState(state);
  }
  else if(key == FTL_STR("uiNodeColor"))
  {
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(value.c_str());
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
    FabricCore::Variant metadataVar = FabricCore::Variant::CreateFromJSON(value.c_str());
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
    QString tooltip = value.c_str();
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
  GraphView::Graph * uiGraph = m_controller->graph();
  GraphView::Node * uiNode = uiGraph->node(nodeName);
  if(!uiNode)
    return;
  uiNode->setTitle(title);
  uiNode->update();
}


void DFGNotificationRouter::onExecPortRenamed(
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName,
  FTL::JSONObject const *execPortJSONObject
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;

  FTL::CStrRef execPortTypeStr =
    execPortJSONObject->getString( FTL_STR("execPortType") );

  if ( execPortTypeStr != FTL_STR("Out") )
  {
    GraphView::SidePanel * uiPanel =
      uiGraph->sidePanel(GraphView::PortType_Input);
    if(!uiPanel)
      return;

    GraphView::Port * uiPort = uiPanel->port(oldPortName);
    if(!uiPort)
      return;
    uiPort->setName(newPortName);
  }

  if ( execPortTypeStr != FTL_STR("In") )
  {
    GraphView::SidePanel * uiPanel =
      uiGraph->sidePanel(GraphView::PortType_Output);
    if(!uiPanel)
      return;

    GraphView::Port * uiPort = uiPanel->port(oldPortName);
    if(!uiPort)
      return;
    uiPort->setName(newPortName);
  }
}

void DFGNotificationRouter::onNodePortRenamed(
  FTL::CStrRef nodeName,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName
  )
{
  // this shouldn't happen for us for now
}

void DFGNotificationRouter::onExecMetadataChanged(
  FTL::CStrRef key,
  FTL::CStrRef value
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;

  // printf("'%s' metadata changed for '%s'\n", key, exec->getExecPath());

  if(key == "uiGraphPan")
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( value );
    FTL::OwnedPtr<FTL::JSONValue const> jsonValue(
      FTL::JSONValue::Decode( jsonStrWithLoc )
      );
    if ( jsonValue )
    {
      FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();
      float x = jsonObject->getFloat64( FTL_STR("x") );
      float y = jsonObject->getFloat64( FTL_STR("y") );
      uiGraph->mainPanel()->setCanvasPan(QPointF(x, y), false);
    }
  }
  else if(key == "uiGraphZoom")
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( value );
    FTL::OwnedPtr<FTL::JSONValue const> jsonValue(
      FTL::JSONValue::Decode( jsonStrWithLoc )
      );
    if ( jsonValue )
    {
      FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();
      float value = jsonObject->getFloat64( FTL_STR("value") );
      // float y = jsonObject->getFloat64( FTL_STR("y") );
      uiGraph->mainPanel()->setCanvasZoom(value, false);
    }
  }
}

void DFGNotificationRouter::onExtDepAdded(
  FTL::CStrRef extension,
  FTL::CStrRef version
  )
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

void DFGNotificationRouter::onExtDepRemoved(
  FTL::CStrRef extension,
  FTL::CStrRef version
  )
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodeCacheRuleChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef newCacheRule
  )
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecCacheRuleChanged(
  FTL::CStrRef newCacheRule
  )
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecPortResolvedTypeChanged(
  FTL::CStrRef portName,
  FTL::CStrRef newResolvedType
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;
  GraphView::Port * uiPort = uiGraph->port(portName);
  if(!uiPort)
    return;

  if(newResolvedType != uiPort->dataType())
  {
    uiPort->setDataType(newResolvedType.data());
    FabricCore::DFGExec exec = getCoreDFGExec();
    uiPort->setColor(m_config.getColorForDataType(newResolvedType, &exec, portName.data()));
    uiGraph->updateColorForConnections(uiPort);
  }
}

void DFGNotificationRouter::onExecPortTypeSpecChanged(
  FTL::CStrRef portPath,
  FTL::CStrRef newTypeSpec
  )
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolvedType
  )
{
  GraphView::Graph * uiGraph = m_controller->graph();
  if(!uiGraph)
    return;

  GraphView::Node * uiNode = uiGraph->node(nodeName);
  if(!uiNode)
    return;
  GraphView::Pin * uiPin = uiNode->pin(portName);
  if(!uiPin)
    return;

  if(newResolvedType != uiPin->dataType())
  {
    FabricCore::DFGExec subExec =
      getCoreDFGExec().getSubExec(nodeName.c_str());
    uiPin->setDataType(newResolvedType.data());
    uiPin->setColor(m_config.getColorForDataType(newResolvedType, &subExec, portName.data()));
    uiGraph->updateColorForConnections(uiPin);
  }
}

void DFGNotificationRouter::onExecPortMetadataChanged(
  FTL::CStrRef portName,
  FTL::CStrRef key,
  FTL::CStrRef value)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortMetadataChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef key,
  FTL::CStrRef value)
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecPortTypeChanged(
  FTL::CStrRef portName,
  FTL::CStrRef execPortType
  )
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onNodePortTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef nodePortType
  )
{
  // todo: we don't do anything here...
}
