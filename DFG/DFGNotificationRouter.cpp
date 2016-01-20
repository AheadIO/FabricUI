// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <FabricUI/DFG/DFGNotificationRouter.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGWidget.h>

#include <FTL/JSONValue.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGNotificationRouter::DFGNotificationRouter(
  DFGController *dfgController,
  const DFGConfig & config
  )
  : m_dfgController( dfgController )
  , m_config( config )
  , m_performChecks( true )
{
  onExecChanged();
}

void DFGNotificationRouter::onExecChanged()
{
  FabricCore::DFGExec &exec = m_dfgController->getExec();
  if ( exec )
    m_coreDFGView = exec.createView( &Callback, this );
  else
    m_coreDFGView = FabricCore::DFGView();
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
        jsonObject->getSInt32( FTL_STR( "portIndex" ) ),
        jsonObject->getString( FTL_STR( "portName" ) ),
        jsonObject->get( FTL_STR("execPortDesc") )->cast<FTL::JSONObject>()
        );
    }
    else if(descStr == FTL_STR("execPortRemoved"))
    {
      onExecPortRemoved(
        jsonObject->getSInt32( FTL_STR( "portIndex" ) ),
        jsonObject->getString( FTL_STR( "portName" ) )
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
    else if(descStr == FTL_STR("instExecTitleChanged"))
    {
      onNodeTitleChanged(
        jsonObject->getString( FTL_STR("instName") ),
        jsonObject->getString( FTL_STR("execTitle") )
        );
    }
    else if(descStr == FTL_STR("nodeRenamed"))
    {
      onNodeRenamed(
        jsonObject->getString( FTL_STR("oldNodeName") ),
        jsonObject->getString( FTL_STR("newNodeName") )
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
    else if(descStr == FTL_STR("nodePortMetadataChanged"))
    {
      onNodePortMetadataChanged(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("portName") ),
        jsonObject->getString( FTL_STR("key") ),
        jsonObject->getString( FTL_STR("value") )
        );
    }
    else if(descStr == FTL_STR("execPortMetadataChanged"))
    {
      onExecPortMetadataChanged(
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
    else if(descStr == FTL_STR("execTitleChanged"))
    {
      onExecTitleChanged(
        jsonObject->getString( FTL_STR("title") )
        );
    }
    else if(descStr == FTL_STR("extDepsChanged"))
    {
      onExecExtDepsChanged(
        jsonObject->getString( FTL_STR("extDeps") )
        );
    }
    else if( descStr == FTL_STR("execPortDefaultValuesChanged") )
    {
      onExecPortDefaultValuesChanged(
        jsonObject->getString( FTL_STR("execPortName") )
        );
    }
    else if( descStr == FTL_STR("nodePortDefaultValuesChanged") )
    {
      onNodePortDefaultValuesChanged(
        jsonObject->getString( FTL_STR("nodeName") ),
        jsonObject->getString( FTL_STR("portName") )
        );
    }
    else if ( descStr == FTL_STR("removedFromOwner") )
    {
      onRemovedFromOwner();
    }
    else if ( descStr == FTL_STR("execPortsReordered") )
    {
      const FTL::JSONArray * newOrder = jsonObject->maybeGetArray( FTL_STR("newOrder") );
      if ( newOrder )
      {
        std::vector<unsigned int> indices;
        for( size_t i = 0; i < newOrder->size(); i++ )
        {
          const FTL::JSONValue * indexVal = newOrder->get( i );
          unsigned int index = indexVal->getSInt32Value();
          indices.push_back( index );
        }

        if( indices.size() > 0 )
          onExecPortsReordered( indices.size(), &indices[ 0 ] );
      }
    }
    else if (descStr == FTL_STR("nodePortsReordered") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      const FTL::JSONArray * newOrder = jsonObject->maybeGetArray( FTL_STR("newOrder") );
      if ( newOrder )
      {
        std::vector<unsigned int> indices;
        for( size_t i = 0; i < newOrder->size(); i++ )
        {
          const FTL::JSONValue * indexVal = newOrder->get( i );
          unsigned int index = indexVal->getSInt32Value();
          indices.push_back( index );
        }

        if( indices.size() > 0 )
          onNodePortsReordered( nodeName, indices.size(), &indices[ 0 ] );
      }
    }
    else if (descStr == FTL_STR("execDidAttachPreset") )
    {
      FTL::CStrRef presetFilePath = jsonObject->getString( FTL_STR("presetFilePath") );
      onExecDidAttachPreset( presetFilePath );
    }
    else if (descStr == FTL_STR("instExecDidAttachPreset") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      FTL::CStrRef presetFilePath = jsonObject->getString( FTL_STR("presetFilePath") );
      onInstExecDidAttachPreset( nodeName, presetFilePath );
    }
    else if (descStr == FTL_STR("execWillDetachPreset") )
    {
      FTL::CStrRef presetFilePath = jsonObject->getString( FTL_STR("presetFilePath") );
      onExecWillDetachPreset( presetFilePath );
    }
    else if (descStr == FTL_STR("instExecWillDetachPreset") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      FTL::CStrRef presetFilePath = jsonObject->getString( FTL_STR("presetFilePath") );
      onInstExecWillDetachPreset( nodeName, presetFilePath );
    }
    else if (descStr == FTL_STR("execEditWouldSplitFromPresetMayHaveChanged") )
    {
      onExecEditWouldSplitFromPresetMayHaveChanged();
    }
    else if (descStr == FTL_STR("instExecEditWouldSplitFromPresetMayHaveChanged") )
    {
      FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
      onInstExecEditWouldSplitFromPresetMayHaveChanged( instName );
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
  FabricCore::DFGExec &exec = m_dfgController->getExec();
  if ( !exec )
    return;

  FabricCore::DFGStringResult desc = exec.getDesc();
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
        i,
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
  FabricCore::DFGExec &exec = m_dfgController->getExec();
  if ( !exec )
    return;

  GraphView::Graph * uiGraph = m_dfgController->graph();
  if(!uiGraph)
    return;

  FabricCore::DFGNodeType nodeType = exec.getNodeType( nodeName.c_str() );
  GraphView::Node * uiNode;
  if ( nodeType == FabricCore::DFGNodeType_User )
    uiNode = uiGraph->addBackDropNode( nodeName );
  else
    uiNode = uiGraph->addNode( nodeName, FTL::CStrRef() );
  if(!uiNode)
    return;

  if(nodeType == FabricCore::DFGNodeType_Var ||
    nodeType == FabricCore::DFGNodeType_Get ||
    nodeType == FabricCore::DFGNodeType_Set)
  {
    uiNode->setColor(m_config.varNodeDefaultColor);
    uiNode->setTitleColor(m_config.varLabelDefaultColor);
  }

  if(nodeType == FabricCore::DFGNodeType_Inst)
  {
    if ( exec.instExecIsPreset( nodeName.c_str() ) )
    {
      FTL::CStrRef title;
      if ( jsonObject->maybeGetString( FTL_STR("execTitle"), title ) )
        uiNode->setTitle( title );
    }
    else
    {
      uiNode->setTitle( nodeName );
      uiNode->setTitleSuffixAsterisk();
    }
  }
  else if(nodeType == FabricCore::DFGNodeType_Var)
  {
    FTL::CStrRef name;
    if ( jsonObject->maybeGetString( FTL_STR("name"), name ) )
      uiNode->setTitle( name );
  }
  else if(nodeType == FabricCore::DFGNodeType_Get || nodeType == FabricCore::DFGNodeType_Set)
  {
    FTL::CStrRef varPath = exec.getRefVarPath( nodeName.c_str() );
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

  if(exec.getNodeType(nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
  {
    FabricCore::DFGExec subExec = exec.getSubExec(nodeName.c_str());
    FTL::CStrRef uiNodeColor = subExec.getMetadata("uiNodeColor");
    if(!uiNodeColor.empty())
      onNodeMetadataChanged(nodeName, "uiNodeColor", uiNodeColor);
    FTL::CStrRef uiHeaderColor = subExec.getMetadata("uiHeaderColor");
    if(!uiHeaderColor.empty())
      onNodeMetadataChanged(nodeName, "uiHeaderColor", uiHeaderColor);
    FTL::CStrRef uiTextColor = subExec.getMetadata("uiTextColor");
    if(!uiTextColor.empty())
      onNodeMetadataChanged(nodeName, "uiTextColor", uiTextColor);
    FTL::CStrRef uiTooltip = subExec.getMetadata("uiTooltip");
    if(!uiTooltip.empty())
      onNodeMetadataChanged(nodeName, "uiTooltip", uiTooltip);
    FTL::CStrRef uiAlwaysShowDaisyChainPorts = subExec.getMetadata("uiAlwaysShowDaisyChainPorts");
    if(!uiAlwaysShowDaisyChainPorts.empty())
      onNodeMetadataChanged(nodeName, "uiAlwaysShowDaisyChainPorts", uiAlwaysShowDaisyChainPorts);
    FTL::CStrRef uiCollapsedState = subExec.getMetadata("uiCollapsedState");
    if(!uiCollapsedState.empty())
      onNodeMetadataChanged(nodeName, "uiCollapsedState", uiCollapsedState);
  }

  if ( exec.getNodeType(nodeName.c_str()) == FabricCore::DFGNodeType_User )
  {
    FTL::CStrRef uiNodeColor =
      exec.getNodeMetadata( nodeName.c_str(), "uiNodeColor" );
    if ( !uiNodeColor.empty() )
      onNodeMetadataChanged( nodeName, "uiNodeColor", uiNodeColor );
    FTL::CStrRef uiHeaderColor =
      exec.getNodeMetadata( nodeName.c_str(), "uiHeaderColor" );
    if ( !uiHeaderColor.empty() )
      onNodeMetadataChanged( nodeName, "uiHeaderColor", uiHeaderColor );
    FTL::CStrRef uiTextColor =
      exec.getNodeMetadata( nodeName.c_str(), "uiTextColor" );
    if ( !uiTextColor.empty() )
      onNodeMetadataChanged( nodeName, "uiTextColor", uiTextColor );
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
    ((DFGController*)m_dfgController)->checkErrors();
  }
}

void DFGNotificationRouter::onNodeRemoved(
  FTL::CStrRef nodeName
  )
{
  GraphView::Graph * uiGraph = m_dfgController->graph();
  if(!uiGraph)
    return;
  GraphView::Node * uiNode = uiGraph->node(nodeName);
  if(!uiNode)
    return;
  uiGraph->removeNode(uiNode);

  // todo - the notif should provide the node type
  // m_dfgController->updatePresetDB(true);

  if(m_performChecks)
  {
    ((DFGController*)m_dfgController)->checkErrors();
  }

  m_dfgController->emitNodeRemoved( nodeName );
}

void DFGNotificationRouter::onNodePortInserted(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::JSONObject const *jsonObject
  )
{
  GraphView::Graph * graph = m_dfgController->graph();
  if(!graph)
    return;
  GraphView::Node * uiNode = graph->node(nodeName);
  if(!uiNode)
    return;

  FTL::CStrRef dataType = jsonObject->getStringOrEmpty( FTL_STR("type") );

  FabricCore::DFGExec &exec = m_dfgController->getExec();

  QColor color;
  FabricCore::DFGExec subExec;
  if(exec.getNodeType(nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
  {
    subExec = exec.getSubExec(nodeName.c_str());
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

  checkAndFixNodePortOrder(subExec, uiNode);  // [FE-5716]
}

void DFGNotificationRouter::onNodePortRemoved(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName
  )
{
  GraphView::Graph * uiGraph = m_dfgController->graph();
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
    ((DFGController*)m_dfgController)->checkErrors();
  }
}

void DFGNotificationRouter::onExecPortInserted(
  int index,
  FTL::CStrRef portName,
  FTL::JSONObject const *jsonObject
  )
{
  FTL::CStrRef dataType = jsonObject->getStringOrEmpty( FTL_STR("type") );

  FabricCore::DFGExec &exec = m_dfgController->getExec();

  if(exec.getType() == FabricCore::DFGExecType_Graph)
  {
    GraphView::Graph * uiGraph = m_dfgController->graph();
    if(!uiGraph)
      return;

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
    if(uiOutPort || uiInPort)
    {
      if(uiOutPort && uiInPort)
        uiGraph->addConnection(uiOutPort, uiInPort, false);
      checkAndFixPanelPortOrder();  // [FE-5716]
    }
  }
  else if(exec.getType() == FabricCore::DFGExecType_Func)
  {
    DFGKLEditorWidget * uiKlEditor = m_dfgController->getDFGWidget()->getKLEditor();
    if(!uiKlEditor)
      return;
    uiKlEditor->onExecChanged();
  }

  m_dfgController->emitArgInserted( index, portName.c_str(), dataType.c_str() );
}

void DFGNotificationRouter::onExecPortRemoved(
  int index,
  FTL::CStrRef portName
  )
{
  FabricCore::DFGExec &exec = m_dfgController->getExec();
  if(exec.getType() == FabricCore::DFGExecType_Graph)
  {
    GraphView::Graph * uiGraph = m_dfgController->graph();
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
      ((DFGController*)m_dfgController)->checkErrors();
    }
  }
  else if(exec.getType() == FabricCore::DFGExecType_Func)
  {
    DFGKLEditorWidget * uiKlEditor = m_dfgController->getDFGWidget()->getKLEditor();
    if(!uiKlEditor)
      return;
    uiKlEditor->onExecChanged();
  }

  m_dfgController->emitArgRemoved( index, portName.c_str() );
}

void DFGNotificationRouter::onPortsConnected(
  FTL::CStrRef srcPath,
  FTL::CStrRef dstPath
  )
{
  GraphView::Graph * uiGraph = m_dfgController->graph();
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
    ((DFGController*)m_dfgController)->checkErrors();
  }

  m_dfgController->emitPortsConnected( srcPath, dstPath );
  m_dfgController->bindUnboundRTVals();
}

void DFGNotificationRouter::onPortsDisconnected(
  FTL::CStrRef srcPath,
  FTL::CStrRef dstPath
  )
{
  GraphView::Graph * uiGraph = m_dfgController->graph();
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
    ((DFGController*)m_dfgController)->checkErrors();
  }
  m_dfgController->emitPortsDisconnected( srcPath, dstPath );
}

void DFGNotificationRouter::onNodeMetadataChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef key,
  FTL::CStrRef value
  )
{
  if(m_dfgController->graph() == NULL)
    return;
  GraphView::Graph * uiGraph = m_dfgController->graph();
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
  else if(key == FTL_STR("uiGraphSize"))
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( value );
    FTL::OwnedPtr<FTL::JSONValue const> jsonValue(
      FTL::JSONValue::Decode( jsonStrWithLoc )
      );
    if ( jsonValue )
    {
      FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();
      float w = jsonObject->getFloat64( FTL_STR("w") );
      float h = jsonObject->getFloat64( FTL_STR("h") );
      if ( uiNode->isBackDropNode() )
      {
        GraphView::BackDropNode *uiBackDropNode =
          static_cast<GraphView::BackDropNode *>( uiNode );
        uiBackDropNode->setSize( QSizeF( w, h ) );
      }
    }
  }
  else if(key == FTL_STR("uiTitle"))
  {
    if ( uiNode->isBackDropNode() )
    {
      GraphView::BackDropNode *uiBackDropNode =
        static_cast<GraphView::BackDropNode *>( uiNode );
      uiBackDropNode->setTitle( value );
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
      if ( uiNode->isBackDropNode() )
      {
        uiNode->setColor( QColor( r, g, b, 0xA0 ) );
        uiNode->setTitleColor( QColor( r, g, b, 0xB0 ) );
      }
      else
      {
        QColor color( r, g, b );
        uiNode->setColor(color);
        uiNode->setTitleColor(color.darker(130));
      }
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
      if ( uiNode->isBackDropNode() )
      {
        uiNode->setTitleColor( QColor( r, g, b, 0xB0 ) );
      }
      else
      {
        QColor color(r, g, b);
        uiNode->setTitleColor(color);
      }
    }
  }
  else if(key == FTL_STR("uiTextColor"))
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
      uiNode->setFontColor(color);
    }
  }
  else if(key == FTL_STR("uiTooltip"))
  {
    QString tooltip = QSTRING_FROM_FTL_UTF8(value);
    uiNode->header()->setToolTip(tooltip.trimmed());
  }
  else if(key == FTL_STR("uiComment"))
  {
    QString text = QSTRING_FROM_FTL_UTF8(value);
    GraphView::NodeBubble *uiBubble = uiNode->bubble();
    if ( text.length() == 0 )
      uiBubble->hide();
    else
      uiBubble->show();
    uiBubble->setText(text);
  }
  else if(key == FTL_STR("uiCommentExpanded"))
  {
    GraphView::NodeBubble * uiBubble = uiNode->bubble();
    if ( value.empty() || value == FTL_STR("false") )
      uiBubble->collapse();
    else
      uiBubble->expand();
  }
  else if(key == FTL_STR("uiAlwaysShowDaisyChainPorts"))
  {
    if(value.size() == 0 || value == "false")
    {
      uiNode->setAlwaysShowDaisyChainPorts(false);
    }
    else
    {
      uiNode->setAlwaysShowDaisyChainPorts(true);
    }
  }
}

void DFGNotificationRouter::onNodeTitleChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef title
  )
{
  if(m_dfgController->graph() == NULL)
    return;
  GraphView::Graph * uiGraph = m_dfgController->graph();
  GraphView::Node * uiNode = uiGraph->node(nodeName);
  if(!uiNode)
    return;

  FabricCore::DFGExec &exec = m_dfgController->getExec();
  if ( exec.getNodeType( nodeName.c_str() ) == FabricCore::DFGNodeType_Inst
    && exec.instExecIsPreset( nodeName.c_str() ) )
  {
    uiNode->setTitle( title );
    uiNode->update();
  }
}


void DFGNotificationRouter::onNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName
  )
{
  GraphView::Graph *uiGraph = m_dfgController->graph();
  if ( !uiGraph )
    return;

  GraphView::Node *uiNode = uiGraph->renameNode( oldNodeName, newNodeName );

  FabricCore::DFGExec &exec = m_dfgController->getExec();
  if ( exec.getNodeType( newNodeName.c_str() ) == FabricCore::DFGNodeType_Inst
    && !exec.instExecIsPreset( newNodeName.c_str() ) )
  {
    assert( !!uiNode );
    uiNode->setTitle( newNodeName );
  }

  m_dfgController->emitNodeRenamed( oldNodeName, newNodeName );
}


void DFGNotificationRouter::onExecPortRenamed(
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName,
  FTL::JSONObject const *execPortJSONObject
  )
{
  GraphView::Graph * uiGraph = m_dfgController->graph();
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

  m_dfgController->emitExecPortRenamed( "", oldPortName, newPortName );
}

void DFGNotificationRouter::onNodePortRenamed(
  FTL::CStrRef nodeName,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName
  )
{
  m_dfgController->emitNodePortRenamed(
    m_dfgController->getExecPath(),
    nodeName,
    oldPortName,
    newPortName
    );
}

void DFGNotificationRouter::onExecMetadataChanged(
  FTL::CStrRef key,
  FTL::CStrRef value
  )
{
  GraphView::Graph * uiGraph = m_dfgController->graph();
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
  if(m_dfgController->graph() == NULL)
    return;
  try
  {
    FabricCore::Client client = ((DFGController*)m_dfgController)->getClient();
    client.loadExtension(extension.data(), version.data(), false);
  }
  catch(FabricCore::Exception e)
  {
    ((DFGController*)m_dfgController)->logError(e.getDesc_cstr());
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
  {
    // Strangely, this notification doesn't come with port
    // index (as Args do), but to notify consistently we need
    // to find it (the index)
    FabricCore::DFGExec exec = m_dfgController->getExec();
    for (int index = 0; index < exec.getExecPortCount(); index++)
    {
      if (strcmp( exec.getExecPortName( index ), portName.c_str() ) == 0)
      {
        m_dfgController->emitArgTypeChanged( index, portName.c_str(), newResolvedType.c_str() );
      }
    }
  }

  GraphView::Graph * uiGraph = m_dfgController->graph();
  if(!uiGraph)
    return;

  // we query for a vector because in case of an io port
  // we might actually have two
  std::vector<GraphView::Port *> uiPorts = uiGraph->ports(portName);
  if(uiPorts.size() == 0)
    return;

  if(newResolvedType != uiPorts[0]->dataType())
  {
    FabricCore::DFGExec &exec = m_dfgController->getExec();
    for(size_t i=0;i<uiPorts.size();i++)
    {
      uiPorts[i]->setDataType(newResolvedType);
      uiPorts[i]->setColor(m_config.getColorForDataType(newResolvedType, &exec, portName.c_str()));
    }
    uiGraph->updateColorForConnections(uiPorts[0]);
  }
}

void DFGNotificationRouter::onExecPortTypeSpecChanged(
  FTL::CStrRef portPath,
  FTL::CStrRef newTypeSpec
  )
{
  FabricCore::DFGExec &exec = m_dfgController->getExec();
  if(exec.getType() == FabricCore::DFGExecType_Func)
  {
    DFGKLEditorWidget * uiKlEditor = m_dfgController->getDFGWidget()->getKLEditor();
    if(!uiKlEditor)
      return;
    uiKlEditor->onExecChanged();
  }
}

void DFGNotificationRouter::onNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolvedType
  )
{
  GraphView::Graph * uiGraph = m_dfgController->graph();
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
    FabricCore::DFGExec &exec = m_dfgController->getExec();
    if(exec.getNodeType(nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
    {
      FabricCore::DFGExec subExec = exec.getSubExec( nodeName.c_str() );
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
  m_dfgController->emitExecPortMetadataChanged(
    portName.c_str(),
    key.c_str(),
    value.c_str() );
}

void DFGNotificationRouter::onNodePortMetadataChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef key,
  FTL::CStrRef value)
{
}

void DFGNotificationRouter::onExecPortTypeChanged(
  FTL::CStrRef portName,
  FTL::CStrRef execPortType
  )
{
  FabricCore::DFGExec &exec = m_dfgController->getExec();
  if(exec.getType() == FabricCore::DFGExecType_Func)
  {
    DFGKLEditorWidget * uiKlEditor = m_dfgController->getDFGWidget()->getKLEditor();
    if(!uiKlEditor)
      return;
    uiKlEditor->onExecChanged();
  }
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
  FabricCore::DFGExec &exec = m_dfgController->getExec();
  if ( !exec )
    return;

  FabricCore::DFGNodeType nodeType = exec.getNodeType(refName.c_str());
  std::string title = newVarPath;
  if(nodeType == FabricCore::DFGNodeType_Get)
    title = "get "+title;
  else if(nodeType == FabricCore::DFGNodeType_Set)
    title = "set "+title;

  GraphView::Graph *uiGraph = m_dfgController->graph();
  if ( !uiGraph )
    return;
  GraphView::Node *uiNode = uiGraph->node( refName );
  if ( !uiNode )
    return;
  uiNode->setTitle( title );
}

void DFGNotificationRouter::onFuncCodeChanged(
  FTL::CStrRef code
  )
{
  // todo: we don't do anything here...
}

void DFGNotificationRouter::onExecTitleChanged(
  FTL::CStrRef title
  )
{
  DFGWidget *dfgWidget = m_dfgController->getDFGWidget();
  dfgWidget->refreshTitle( title );
}

void DFGNotificationRouter::onExecExtDepsChanged(
  FTL::CStrRef extDeps
  )
{
  DFGWidget *dfgWidget = m_dfgController->getDFGWidget();
  dfgWidget->refreshExtDeps( extDeps );
}

void DFGNotificationRouter::onNodePortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName
  )
{
  m_dfgController->emitDefaultValuesChanged( -1, portName.c_str() );
}

void DFGNotificationRouter::onExecPortDefaultValuesChanged(
  FTL::CStrRef portName
  )
{
  m_dfgController->emitDefaultValuesChanged( -1, portName.c_str() );
}

void DFGNotificationRouter::onRemovedFromOwner()
{
  FTL::StrRef execPath = m_dfgController->getExecPath();
  if ( execPath.empty() )
    return;

  // [FE-5435] we need to go up as high as required
  // the level above might not exist anymore either.
  while(true)
  {
    if(execPath.empty())
      break;
    
    FTL::StrRef::Split split = execPath.rsplit('.');

    std::string parentExecPath;
    if( split.first.empty() || split.second.empty() )
      parentExecPath = "";
    else
      parentExecPath = split.first;

    FabricCore::DFGExec parentExec;
    try
    {
      FabricCore::DFGBinding &binding = m_dfgController->getBinding();
      FabricCore::DFGExec rootExec = binding.getExec();
      if(parentExecPath.empty())
        parentExec = rootExec;
      else
        parentExec = rootExec.getSubExec( parentExecPath.c_str() );
    }
    catch ( FabricCore::Exception e )
    {
      // the sub exec does not exist... let's go up higher
      execPath = parentExecPath;
      continue;
    }

    m_dfgController->setExec( parentExecPath, parentExec );
    break;
  }
}

void DFGNotificationRouter::onExecPortsReordered(
  unsigned int indexCount,
  unsigned int * indices
  )
{
  GraphView::Graph * uiGraph = m_dfgController->graph();
  if(!uiGraph)
    return;

  GraphView::SidePanel * leftPanel = uiGraph->sidePanel(GraphView::PortType_Input);
  GraphView::SidePanel * rightPanel = uiGraph->sidePanel(GraphView::PortType_Output);

  FabricCore::DFGExec & exec = m_dfgController->getExec();
  QStringList inputs, outputs;
  for(unsigned int i=0;i<exec.getExecPortCount();i++)
  {
    QString name = exec.getExecPortName(i);
    if(exec.getExecPortType(i) != FabricCore::DFGPortType_Out)
      inputs.append(name);
    if(exec.getExecPortType(i) != FabricCore::DFGPortType_In)
      outputs.append(name);
  }

  leftPanel->reorderPorts(outputs);
  rightPanel->reorderPorts(inputs);
}

void DFGNotificationRouter::onNodePortsReordered(
  FTL::CStrRef nodeName,
  unsigned int indexCount,
  unsigned int * indices
  )
{
  GraphView::Graph * uiGraph = m_dfgController->graph();
  if(!uiGraph)
    return;

  GraphView::Node * uiNode = uiGraph->node(nodeName);
  if(!uiNode)
    return;

  FabricCore::DFGExec & exec = m_dfgController->getExec();
  FabricCore::DFGExec subExec = exec.getSubExec(nodeName.c_str());
  QStringList names;
  for(unsigned int i=0;i<subExec.getExecPortCount();i++)
  {
    QString name = subExec.getExecPortName(i);
    names.append(name);
  }

  uiNode->reorderPins(names);
}

void DFGNotificationRouter::onExecDidAttachPreset(
  FTL::CStrRef presetFilePath
  )
{
  // nothing to be done here for now.
}

void DFGNotificationRouter::onInstExecDidAttachPreset(
  FTL::CStrRef nodeName,
  FTL::CStrRef presetFilePath
  )
{
  // nothing to be done here for now.
}

void DFGNotificationRouter::onExecWillDetachPreset(
  FTL::CStrRef presetFilePath
  )
{
  // nothing to be done here for now.
}

void DFGNotificationRouter::onInstExecWillDetachPreset(
  FTL::CStrRef nodeName,
  FTL::CStrRef presetFilePath
  )
{
  // nothing to be done here for now.
}

void DFGNotificationRouter::onExecEditWouldSplitFromPresetMayHaveChanged()
{
  m_dfgController->emitExecSplitChanged();
}

void DFGNotificationRouter::onInstExecEditWouldSplitFromPresetMayHaveChanged(
  FTL::CStrRef instName
  )
{
  GraphView::Graph * uiGraph = m_dfgController->graph();
  if(!uiGraph)
    return;
  GraphView::Node * uiNode = uiGraph->node(instName);
  if(!uiNode)
    return;

  FabricCore::DFGExec & exec = m_dfgController->getExec();
  FabricCore::DFGExec subExec = exec.getSubExec( instName.c_str() );
  if ( !subExec.isPreset() )
  {
    uiNode->setTitle( instName );
    uiNode->setTitleSuffixAsterisk();
  }
  else
  {
    uiNode->setTitle( subExec.getTitle() );
    uiNode->removeTitleSuffix();
  }
}

void DFGNotificationRouter::checkAndFixPanelPortOrder()
{
  // get graph and exec.
  GraphView::Graph    *uiGraph = m_dfgController->graph();
  FabricCore::DFGExec &exec    = m_dfgController->getExec();
  if (!uiGraph || !exec.isValid() || !exec.getExecPortCount())
    return;

  // check if all exec ports are in uiGraph.
  for (int i=exec.getExecPortCount()-1;i>=0;i--)
    if (!uiGraph->port(exec.getExecPortName(i)))
      return;

  // check/fix the panels.
  for (int pass=0;pass<2;pass++)
  {
    GraphView::SidePanel *panel = uiGraph->sidePanel(pass == 0 ? GraphView::PortType_Input : GraphView::PortType_Output);
    FabricCore::DFGPortType portType = (pass == 0 ? FabricCore::DFGPortType_In : FabricCore::DFGPortType_Out);
    if (!panel) continue;

    // create the correct list of names based on the ports in exec.
    QStringList correctNames;
    for (unsigned int i=0;i<exec.getExecPortCount();i++)
      if (exec.getExecPortType(i) != portType)
        correctNames.append(QString(exec.getExecPortName(i)));

    // check if the panel's port order mismatches and reorder the ports if necessary.
    if (size_t(panel->portCount()) == size_t(correctNames.count()))
    {
      for (int i=0;i<correctNames.count();i++)
        if (panel->port(i)->name() != correctNames.at(i).toLocal8Bit().constData())
        {
          panel->reorderPorts(correctNames);
          break;
        }
    }
  }
}

void DFGNotificationRouter::checkAndFixNodePortOrder(FabricCore::DFGExec &nodeExec, GraphView::Node *uiNode)
{
  // check inputs.
  if (!nodeExec.isValid() || !uiNode)
    return;

  // check if all exec ports are in uiNode.
  for (int i=nodeExec.getExecPortCount()-1;i>=0;i--)
    if (!uiNode->pin(nodeExec.getExecPortName(i)))
      return;

  // create the correct list of names based on the ports in nodeExec.
  QStringList correctNames;
  for (unsigned int i=0;i<nodeExec.getExecPortCount();i++)
    correctNames.append(QString(nodeExec.getExecPortName(i)));

  // check if the uiNode's pin order mismatches and reorder them if necessary.
  if (size_t(uiNode->pinCount()) == size_t(correctNames.count()))
    for (int i=0;i<correctNames.count();i++)
      if (uiNode->pin(i)->name() != correctNames.at(i).toLocal8Bit().constData())
      {
        uiNode->reorderPins(correctNames);
        break;
      }
}
