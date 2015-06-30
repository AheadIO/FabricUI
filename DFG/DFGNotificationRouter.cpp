// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include "DFGNotificationRouter.h"
#include "DFGController.h"

#include <FTL/JSONValue.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGNotificationRouter::DFGNotificationRouter(
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec,
  const DFGConfig & config
  )
  : m_binding( binding )
  , m_execPath( execPath )
  , m_exec( exec )
  , m_view( exec.createView( &Callback, this ) )
  , m_controller(NULL)
  , m_config( config )
  , m_performChecks( true )
{
}

void DFGNotificationRouter::callback( FTL::CStrRef jsonStr )
{
  try
  {
    // printf( "notif = %s\n", jsonStr.c_str() );
    
    // FabricCore::DFGStringResult desc = m_exec.getDesc();
    // printf( "exec = %s\n", desc.getCString() );

    onNotification(jsonStr);

    FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );
    FTL::OwnedPtr<FTL::JSONObject> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
      );
    FTL::StrRef descStr = jsonObject->getString( FTL_STR("desc") );

    if(descStr == FTL_STR("nodeInserted"))
    {
      onNodeInserted(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->get( FTL_STR("nodeDesc") )->cast<FTL::JSONObject>()
        );
    }
    else if(descStr == FTL_STR("nodeRemoved"))
    {
      onNodeRemoved(
        jsonObject->getString( FTL_STR("nodeName") )
        );
    }
    else if(descStr == FTL_STR("nodePortInserted"))
    {
      onNodePortInserted(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("portName") ),
        jsonObject->get( FTL_STR("nodePortDesc") )->cast<FTL::JSONObject>()
        );
    }
    else if(descStr == FTL_STR("nodePortRemoved"))
    {
      onNodePortRemoved(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("portName") )
        );
    }
    else if(descStr == FTL_STR("execPortInserted"))
    {
      onExecPortInserted(
        jsonObject->getString( FTL_STR("portName") ),
        jsonObject->get( FTL_STR("execPortDesc") )->cast<FTL::JSONObject>()
        );
    }
    else if(descStr == FTL_STR("execPortRemoved"))
    {
      onExecPortRemoved(
        jsonObject->getString( FTL_STR("portName") )
        );
    }
    else if(descStr == FTL_STR("portsConnected"))
    {
      onPortsConnected(
        jsonObject->getString( FTL_STR("srcPath") ),
        jsonObject->getString( FTL_STR("dstPath") )
        );
    }
    else if(descStr == FTL_STR("portsDisconnected"))
    {
      onPortsDisconnected(
        jsonObject->getString( FTL_STR("srcPath") ),
        jsonObject->getString( FTL_STR("dstPath") )
        );
    }
    else if(descStr == FTL_STR("nodeMetadataChanged"))
    {
      onNodeMetadataChanged(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("key") ),
        jsonObject->getString( FTL_STR("value") )
        );
    }
    else if(descStr == FTL_STR("nodeTitleChanged"))
    {
      onNodeTitleChanged(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("title") )
        );
    }
    else if(descStr == FTL_STR("execPortRenamed"))
    {
      onExecPortRenamed(
        jsonObject->getString( FTL_STR("oldPortName") ),
        jsonObject->getString( FTL_STR("newPortName") ),
        jsonObject->get( FTL_STR("execPortDesc") )->cast<FTL::JSONObject>()
        );
    }
    else if(descStr == FTL_STR("nodePortRenamed"))
    {
      onNodePortRenamed(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("oldPortName") ),
        jsonObject->getString( FTL_STR("newPortName") )
        );
    }
    else if(descStr == FTL_STR("execMetadataChanged"))
    {
      onExecMetadataChanged(
        jsonObject->getString( FTL_STR("key") ),
        jsonObject->getString( FTL_STR("value") )
        );
    }
    else if(descStr == FTL_STR("extDepAdded"))
    {
      onExtDepAdded(
        jsonObject->getString( FTL_STR("name") ),
        jsonObject->getString( FTL_STR("versionRange") )
        );
    }
    else if(descStr == FTL_STR("extDepRemoved"))
    {
      onExtDepRemoved(
        jsonObject->getString( FTL_STR("name") ),
        jsonObject->getString( FTL_STR("versionRange") )
        );
    }
    else if(descStr == FTL_STR("nodeCacheRuleChanged"))
    {
      onNodeCacheRuleChanged(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("cacheRule") )
        );
    }
    else if(descStr == FTL_STR("execCacheRuleChanged"))
    {
      onExecCacheRuleChanged(
        jsonObject->getString( FTL_STR("cacheRule") )
        );
    }
    else if(descStr == FTL_STR("execPortResolvedTypeChanged"))
    {
      onExecPortResolvedTypeChanged(
        jsonObject->getString( FTL_STR("portName") ),
        jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") )
        );
    }
    else if(descStr == FTL_STR("execPortTypeSpecChanged"))
    {
      onExecPortTypeSpecChanged(
        jsonObject->getString( FTL_STR("portName") ),
        jsonObject->getStringOrEmpty( FTL_STR("newTypeSpec") )
        );
    }
    else if(descStr == FTL_STR("nodePortResolvedTypeChanged"))
    {
      onNodePortResolvedTypeChanged(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("portName") ),
        jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") )
        );
    }
    else if(descStr == FTL_STR("portMetadataChanged"))
    {
      onExecPortMetadataChanged(
        jsonObject->getString( FTL_STR("portName") ),
        jsonObject->getString( FTL_STR("key") ),
        jsonObject->getString( FTL_STR("value") )
        );
    }
    else if(descStr == FTL_STR("nodePortMetadataChanged"))
    {
      onNodePortMetadataChanged(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("portName") ),
        jsonObject->getString( FTL_STR("key") ),
        jsonObject->getString( FTL_STR("value") )
        );
    }
    else if(descStr == FTL_STR("execPortTypeChanged"))
    {
      onExecPortTypeChanged(
        jsonObject->getString( FTL_STR("portName") ),
        jsonObject->getString( FTL_STR("newExecPortType") )
        );
    }
    else if(descStr == FTL_STR("nodePortTypeChanged"))
    {
      onNodePortTypeChanged(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("portName") ),
        jsonObject->getString( FTL_STR("newNodePortType") )
        );
    }
    else if(descStr == FTL_STR("refVarPathChanged"))
    {
      onRefVarPathChanged(
        jsonObject->getString( FTL_STR("refName") ),
        jsonObject->getString( FTL_STR("newVarPath") )
        );
    }
    else if(descStr == FTL_STR("funcCodeChanged"))
    {
      onFuncCodeChanged(
        jsonObject->getString( FTL_STR("code") )
        );
    }
    else
    {
      printf(
        "NotificationRouter::callback: Unhandled notification:\n%s\n",
        jsonStr.data()
        );
    }
  }
  catch ( FabricCore::Exception e )
  {
    printf(
      "NotificationRouter::callback: caught Core exception: %s\n",
      e.getDesc_cstr()
      );
  }
  catch ( FTL::JSONException e )
  {
    printf(
      "NotificationRouter::callback: caught FTL::JSONException: %s\n",
      e.getDescCStr()
      );
  }
}

void DFGNotificationRouter::onGraphSet()
{
  if(!m_exec)
    return;

  FabricCore::DFGStringResult desc = m_exec.getDesc();
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

    if ( rootObject->getString( FTL_STR("objectType") ) == FTL_STR("Graph") )
    {
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

    m_controller->emitStructureChanged();
    m_controller->emitRecompiled();
  }
  catch ( FTL::JSONException je )
  {
    printf( "Caught JSONException: %s\n", je.getDescCStr() );
  }
}

void DFGNotificationRouter::onNotification(FTL::CStrRef json)
{
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
    uiGraph->addNode( nodeName, FTL::CStrRef() );
  if(!uiNode)
    return;

  FabricCore::DFGNodeType nodeType = m_exec.getNodeType(nodeName.c_str());
  if(nodeType == FabricCore::DFGNodeType_Var ||
    nodeType == FabricCore::DFGNodeType_Get ||
    nodeType == FabricCore::DFGNodeType_Set)
  {
    uiNode->setColor(m_config.varNodeDefaultColor);
    uiNode->setTitleColor(m_config.varLabelDefaultColor);
  }

  FTL::CStrRef title;
  if(nodeType == FabricCore::DFGNodeType_Inst)
  {
    if ( jsonObject->maybeGetString( FTL_STR("title"), title ) )
      onNodeTitleChanged( nodeName, title );
  }
  else if(nodeType == FabricCore::DFGNodeType_Var)
  {
    if ( jsonObject->maybeGetString( FTL_STR("name"), title ) )
      onNodeTitleChanged( nodeName, title );
  }
  else if(nodeType == FabricCore::DFGNodeType_Get || nodeType == FabricCore::DFGNodeType_Set)
  {
    FTL::CStrRef varPath = m_exec.getRefVarPath(nodeName.c_str());
    onRefVarPathChanged(nodeName, varPath);
  }

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

  if(m_exec.getNodeType(nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
  {
    FabricCore::DFGExec subExec = m_exec.getSubExec(nodeName.c_str());
    FTL::CStrRef uiNodeColor = subExec.getMetadata("uiNodeColor");
    if(!uiNodeColor.empty())
      onNodeMetadataChanged(nodeName, "uiNodeColor", uiNodeColor);
    FTL::CStrRef uiHeaderColor = subExec.getMetadata("uiHeaderColor");
    if(!uiHeaderColor.empty())
      onNodeMetadataChanged(nodeName, "uiHeaderColor", uiHeaderColor);
    FTL::CStrRef uiTooltip = subExec.getMetadata("uiTooltip");
    if(!uiTooltip.empty())
      onNodeMetadataChanged(nodeName, "uiTooltip", uiTooltip);
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

  m_controller->emitStructureChanged();
  m_controller->emitRecompiled();
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

  // todo - the notif should provide the node type
  // m_controller->updatePresetDB(true);

  if(m_performChecks)
  {
    ((DFGController*)m_controller)->checkErrors();
  }

  m_controller->emitStructureChanged();
  m_controller->emitRecompiled();
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

  FabricCore::DFGExec exec = getExec();

  QColor color;
  if(exec.getNodeType(nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
  {
    FabricCore::DFGExec subExec = exec.getSubExec(nodeName.c_str());
    color = m_config.getColorForDataType(dataType, &subExec, portName.c_str());
  }
  else
    color = m_config.getColorForDataType(dataType);

  FTL::CStrRef nodePortType =
    jsonObject->getStringOrEmpty( FTL_STR("nodePortType") );
  GraphView::PortType pType = GraphView::PortType_Input;
  if(nodePortType == FTL_STR("Out"))
    pType = GraphView::PortType_Output;
  else if(nodePortType == FTL_STR("IO"))
    pType = GraphView::PortType_IO;

  GraphView::Pin * uiPin = new GraphView::Pin(uiNode, portName.c_str(), pType, color, portName.c_str());
  if ( !dataType.empty() )
    uiPin->setDataType(dataType);
  uiNode->addPin(uiPin, false);

  m_controller->emitStructureChanged();
  m_controller->emitRecompiled();
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

  m_controller->emitStructureChanged();
  m_controller->emitRecompiled();
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

  FabricCore::DFGExec exec = getExec();
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
  }
  if(uiOutPort && uiInPort)
  {
    uiGraph->addConnection(uiOutPort, uiInPort, false);
  }

  m_controller->emitStructureChanged();
  m_controller->emitRecompiled();
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

  m_controller->emitStructureChanged();
  m_controller->emitRecompiled();
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

  m_controller->bindUnboundRTVals();
  m_controller->emitArgsChanged();
  m_controller->emitStructureChanged();
  m_controller->emitRecompiled();
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

  m_controller->emitArgsChanged();
  m_controller->emitStructureChanged();
  m_controller->emitRecompiled();
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
    FTL::JSONStrWithLoc jsonStrWithLoc( value );
    FTL::OwnedPtr<FTL::JSONValue const> jsonValue(
      FTL::JSONValue::Decode( jsonStrWithLoc )
      );
    if ( jsonValue )
    {
      FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();
      int r = int(jsonObject->getFloat64( FTL_STR("r") ));
      int g = int(jsonObject->getFloat64( FTL_STR("g") ));
      int b = int(jsonObject->getFloat64( FTL_STR("b") ));
      QColor color(r, g, b);
      uiNode->setColor(color);
      uiNode->setTitleColor(color.darker(130));
    }
  }
  else if(key == FTL_STR("uiHeaderColor"))
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( value );
    FTL::OwnedPtr<FTL::JSONValue const> jsonValue(
      FTL::JSONValue::Decode( jsonStrWithLoc )
      );
    if ( jsonValue )
    {
      FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();
      int r = int(jsonObject->getFloat64( FTL_STR("r") ));
      int g = int(jsonObject->getFloat64( FTL_STR("g") ));
      int b = int(jsonObject->getFloat64( FTL_STR("b") ));
      QColor color(r, g, b);
      uiNode->setTitleColor(color);
    }
  }
  else if(key == FTL_STR("uiTooltip"))
  {
    QString tooltip = value.c_str();
    uiNode->header()->setToolTip(tooltip.trimmed());
  }
  else if(key == FTL_STR("uiComment"))
  {
    QString text = value.c_str();
    GraphView::NodeBubble * uiBubble = uiNode->bubble();
    if(text.length() == 0)
    {
      if(uiBubble != NULL)
      {
        uiBubble->setNode(NULL);
        uiNode->setBubble(NULL);
        uiBubble->scene()->removeItem(uiBubble);
        uiBubble->hide();
        uiBubble->deleteLater();
      }
    }
    else
    {
      if(uiBubble == NULL)
        uiBubble = new GraphView::NodeBubble(uiNode->graph(), uiNode, uiNode->graph()->config());
      uiBubble->setText(text);
    }
  }
  else if(key == FTL_STR("uiCommentExpanded"))
  {
    GraphView::NodeBubble * uiBubble = uiNode->bubble();
    if(uiBubble)
    {
      if(value.size() == 0 || value == "false")
        uiBubble->collapse();
      else
        uiBubble->expand();
    }
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

  if ( execPortTypeStr != FTL_STR("In") )
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

  if ( execPortTypeStr != FTL_STR("Out") )
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
  else if(key == "uiBackDrops")
  {
    // we ignore this for now,
    // it's just the list of names
  }
  else if(key.startswith("uiBackDrop_"))
  {
    if(value.empty())
    {
      FTL::CStrRef name = key.substr(11).data();
      GraphView::BackDropNode * uiNode = (GraphView::BackDropNode*)uiGraph->node(name.c_str());
      if(uiNode)
      {
        uiGraph->removeNode(uiNode);
      }
    }
    else
    {
      FTL::JSONStrWithLoc jsonStrWithLoc( value );
      FTL::OwnedPtr<FTL::JSONValue const> jsonValue(
        FTL::JSONValue::Decode( jsonStrWithLoc )
        );
      if ( jsonValue )
      {
        FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();

        FTL::CStrRef name = jsonObject->getString( FTL_STR("name") );

        GraphView::BackDropNode * uiNode = (GraphView::BackDropNode*)uiGraph->node(name.c_str());
        if(!uiNode)
        {
          FTL::CStrRef title = jsonObject->getString( FTL_STR("title") );
          uiNode = new GraphView::BackDropNode(uiGraph, name.c_str(), title.c_str());
          uiNode->setColor(uiNode->color()); // force transparency to work
          uiGraph->addNode(uiNode);
        }

        uiNode->updateFromJSON(jsonObject);
      }
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
    uiPort->setDataType(newResolvedType);
    FabricCore::DFGExec exec = getExec();
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
    uiPin->setDataType(newResolvedType);
    if(getExec().getNodeType(nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
    {
      FabricCore::DFGExec subExec = getExec().getSubExec( nodeName.c_str() );
      uiPin->setColor(m_config.getColorForDataType(newResolvedType, &subExec, portName.data()));
    }
    else
      uiPin->setColor(m_config.getColorForDataType(newResolvedType));
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

void DFGNotificationRouter::onRefVarPathChanged(
  FTL::CStrRef refName,
  FTL::CStrRef newVarPath
  )
{
  FabricCore::DFGNodeType nodeType = m_exec.getNodeType(refName.c_str());
  std::string title = newVarPath;
  if(nodeType == FabricCore::DFGNodeType_Get)
    title = "get "+title;
  else if(nodeType == FabricCore::DFGNodeType_Set)
    title = "set "+title;
  onNodeTitleChanged(refName, title.c_str());
}

void DFGNotificationRouter::onFuncCodeChanged(
  FTL::CStrRef code
  )
{
  // todo: we don't do anything here...
}
