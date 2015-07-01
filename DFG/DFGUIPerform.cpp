//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUIPerform.h>
#include <FabricUI/GraphView/BackDropNode.h>
#include <FTL/JSONValue.h>

#include <sstream>

FABRIC_UI_DFG_NAMESPACE_BEGIN

std::string DFGUIPerform_AddPort(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desiredPortName,
  FabricCore::DFGPortType portType,
  FTL::CStrRef typeSpec,
  FTL::CStrRef portToConnect,
  unsigned &coreUndoCount
  )
{
  FTL::CStrRef portName =
    exec.addExecPort(
      desiredPortName.c_str(),
      portType,
      typeSpec.c_str()
      );
  ++coreUndoCount;

  if ( !portToConnect.empty() )
  {
    FabricCore::DFGPortType portToConnectNodePortType =
      exec.getNodePortType( portToConnect.c_str() );

    if ( portToConnectNodePortType == FabricCore::DFGPortType_In )
    {
      std::pair<FTL::StrRef, FTL::CStrRef> split = portToConnect.rsplit('.');
      std::string nodeToConnect = split.first;

      if ( !nodeToConnect.empty()
        && exec.getNodeType( nodeToConnect.c_str() )
          == FabricCore::DFGNodeType_Inst )
      {
        FTL::CStrRef nodePortToConnect = split.second;

        FTL::CStrRef resolvedType =
          exec.getNodePortResolvedType( portToConnect.c_str() );
        if ( !resolvedType.empty() )
        {
          FabricCore::RTVal defaultValue =
            exec.getInstPortResolvedDefaultValue(
              portToConnect.c_str(),
              resolvedType.c_str()
              );
          if ( defaultValue.isValid() )
          {
            if ( execPath.empty() )
              binding.setArgValue( portName.c_str(), defaultValue );
            else
              exec.setPortDefaultValue( portName.c_str(), defaultValue );
            ++coreUndoCount;
          }
        }

        FabricCore::DFGExec subExec =
          exec.getSubExec( nodeToConnect.c_str() );

        char const *metadatasToCopy[3] =
        {
          "uiRange",
          "uiCombo",
          "uiHidden"
        };

        for ( unsigned i = 0; i < 3; ++i )
        {
          exec.setExecPortMetadata(
            portName.c_str(),
            metadatasToCopy[i],
            subExec.getExecPortMetadata(
              nodePortToConnect.c_str(),
              metadatasToCopy[i]
              ),
            true
            );
          ++coreUndoCount;
        }
      }
    }

    if ( portType != FabricCore::DFGPortType_Out
      && portToConnectNodePortType != FabricCore::DFGPortType_Out )
    {
      exec.connectTo( portName.c_str(), portToConnect.c_str() );
      ++coreUndoCount;
    }
    if ( portType != FabricCore::DFGPortType_In
      && portToConnectNodePortType != FabricCore::DFGPortType_In )
    {
      exec.connectTo( portToConnect.c_str(), portName.c_str() );
      ++coreUndoCount;
    }
  }
  else if ( execPath.empty()
    && !typeSpec.empty()
    && typeSpec.find('$') == typeSpec.end() )
  {
    FabricCore::DFGHost host = binding.getHost();
    FabricCore::Context context = host.getContext();
    FabricCore::RTVal argValue =
      FabricCore::RTVal::Construct(
        context,
        typeSpec.c_str(),
        0,
        0
        );
    binding.setArgValue(
      portName.c_str(),
      argValue
      );
    ++coreUndoCount;
  }

  return portName;
}

void DFGUIPerform_RemovePort(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef portName,
  unsigned &coreUndoCount
  )
{
  exec.removeExecPort( portName.c_str() );
  ++coreUndoCount;
}

static QPoint GetNodeUIGraphPos( 
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName
  )
{
  QPoint result;
  FTL::CStrRef uiGraphPosJSON =
    exec.getNodeMetadata( nodeName.c_str(), "uiGraphPos" );
  if ( !uiGraphPosJSON.empty() )
  {
    FTL::OwnedPtr<FTL::JSONValue const> uiGraphPosJV(
      FTL::JSONValue::Decode( uiGraphPosJSON )
      );

    if ( FTL::JSONObject const *uiGraphPosJO =
      uiGraphPosJV->maybeCast<FTL::JSONObject>() )
    {
      if ( FTL::JSONValue const *xJV = uiGraphPosJO->maybeGet( "x" ) )
      {
        switch ( xJV->getType() )
        {
          case FTL::JSONValue::Type_SInt32:
            result.setX( xJV->getSInt32Value() );
            break;

          case FTL::JSONValue::Type_Float64:
            result.setX( xJV->getFloat64Value() );
            break;

          default: break;
        }
      }

      if ( FTL::JSONValue const *yJV = uiGraphPosJO->maybeGet( "y" ) )
      {
        switch ( yJV->getType() )
        {
          case FTL::JSONValue::Type_SInt32:
            result.setY( yJV->getSInt32Value() );
            break;

          case FTL::JSONValue::Type_Float64:
            result.setY( yJV->getFloat64Value() );
            break;

          default: break;
        }
      }
    }
  }
  return result;
}

void DFGUIPerform_MoveNode(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  QPoint pos,
  unsigned &coreUndoCount
  )
{
  std::string json;
  {
    FTL::JSONEnc<std::string> je( json, FTL::JSONFormat::Packed() );
    FTL::JSONObjectEnc<std::string> joe( je );
    {
      FTL::JSONEnc<std::string> xJE( joe, FTL_STR("x") );
      FTL::JSONFloat64Enc<std::string> xJFE( xJE, pos.x() );
    }
    {
      FTL::JSONEnc<std::string> yJE( joe, FTL_STR("y") );
      FTL::JSONFloat64Enc<std::string> yJFE( yJE, pos.y() );
    }
  }
  exec.setNodeMetadata( nodeName.c_str(), "uiGraphPos", json.c_str(), true );
  ++coreUndoCount;
}

std::string DFGUIPerform_ImplodeNodes(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desiredNodeName,
  FTL::ArrayRef<FTL::CStrRef> const &nodeNames,
  unsigned &coreUndoCount
  )
{
  size_t count = nodeNames.size();

  QPoint oldCenter;
  if ( count > 0 )
  {
    for ( size_t i = 0; i < count; ++i )
      oldCenter += GetNodeUIGraphPos( exec, nodeNames[i] );
    oldCenter /= count;
  }

  char const *nodeNameCStrs[count];
  for ( size_t i = 0; i < count; ++i )
    nodeNameCStrs[i] = nodeNames[i].c_str();

  FTL::CStrRef newNodeName =
    exec.implodeNodes(
      desiredNodeName.c_str(),
      count,
      nodeNameCStrs
      );
  ++coreUndoCount;

  exec.setInstTitle(
    newNodeName.c_str(),
    desiredNodeName.c_str()
    );
  ++coreUndoCount;

  if ( count > 0 )
    DFGUIPerform_MoveNode(
      binding,
      execPath,
      exec,
      newNodeName,
      oldCenter,
      coreUndoCount
      );

  return newNodeName;
}

std::vector<std::string> DFGUIPerform_ExplodeNode(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  unsigned &coreUndoCount
  )
{
  QPoint oldPosition = GetNodeUIGraphPos( exec, nodeName );

  FabricCore::DFGStringResult newNodeNamesJSON =
    exec.explodeNode( nodeName.c_str() );
  ++coreUndoCount;

  FTL::OwnedPtr<FTL::JSONArray const> newNodeNamesJA(
    FTL::JSONValue::Decode(
      newNodeNamesJSON.getCString()
      )->cast<FTL::JSONArray>()
    );

  std::vector<std::string> newNodeNames;
  size_t count = newNodeNamesJA->size();
  newNodeNames.reserve( count );
  for ( size_t i = 0; i < count; ++i )
  {
    FTL::JSONValue const *newNodeNameJV = newNodeNamesJA->get( i );
    newNodeNames.push_back( newNodeNameJV->getStringValue() );
  }

  if ( !newNodeNames.empty() )
  {
    std::vector<QPoint> newPositions;
    newPositions.reserve( newNodeNames.size() );
    for ( size_t i = 0; i < count; ++i )
      newPositions.push_back(
        GetNodeUIGraphPos( exec, newNodeNames[i] )
        );

    QPoint newCenter;
    for ( size_t i = 0; i < count; ++i )
      newCenter += newPositions[i];
    newCenter /= count;

    for ( size_t i = 0; i < count; ++i )
      DFGUIPerform_MoveNode(
        binding,
        execPath,
        exec,
        newNodeNames[i],
        oldPosition + newPositions[i] - newCenter,
        coreUndoCount
        );
  }

  return newNodeNames;
}

void DFGUIPerform_AddBackDrop(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef title,
  unsigned &coreUndoCount
  )
{
  std::string uiBackDropsStr = exec.getMetadata( "uiBackDrops" );

  FTL::OwnedPtr<FTL::JSONArray> uiBackDropsJSON;
  if ( !uiBackDropsStr.empty() && uiBackDropsStr[0] != '[' )
  {
    // [pzion 20150701] Upgrade from broken old format
    std::vector<FTL::StrRef> uiBackDrops;
    std::pair<FTL::StrRef, FTL::CStrRef> split = uiBackDropsStr.split(',');
    while ( !split.first.empty() )
    {
      uiBackDrops.push_back( split.first );
      split = split.second.split(',');
    }
  }
  else
  {
    FTL::JSONStrWithLoc strWithLoc( uiBackDropsStr );
    uiBackDropsJSON = JSONValue::Decode( strWithLoc )->cast<FTL::JSONArray>();
  }

  std::stringstream nameStr;
  nameStr << title;
  nameStr << '_';
  nameStr << uiBackDrops.size();

  std::string keyStr = "uiBackDrop_" + nameStr.str();
  QString jsonStr =
    GraphView::BackDropNode::getDefaultJSON( nameStr.str(), title, pos );

  if ( !uiBackDropsStr.empty() )
    uiBackDropsStr += ',';
  uiBackDropsStr += keyStr;

  exec.setMetadata( "uiBackDrops", uiBackDropsStr.c_str(), true );
  ++coreUndoCount;

  exec.setMetadata( keyStr.c_str(), jsonStr.toUtf8().constData(), true );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
