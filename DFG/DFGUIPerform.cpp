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

static QPointF GetNodeUIGraphPos( 
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName
  )
{
  QPointF result;
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

void DFGUIPerform_MoveNodes(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::ArrayRef<std::string> nodeNames,
  FTL::ArrayRef<QPointF> newTopLeftPoss,
  unsigned &coreUndoCount
  )
{
  for ( size_t i = 0; i < nodeNames.size(); ++i )
  {
    FTL::CStrRef nodeName = nodeNames[i];
    QPointF newTopLeftPos = newTopLeftPoss[i];

    std::string json;
    {
      FTL::JSONEnc<std::string> je( json, FTL::JSONFormat::Packed() );
      FTL::JSONObjectEnc<std::string> joe( je );
      {
        FTL::JSONEnc<std::string> xJE( joe, FTL_STR("x") );
        FTL::JSONFloat64Enc<std::string> xJFE( xJE, newTopLeftPos.x() );
      }
      {
        FTL::JSONEnc<std::string> yJE( joe, FTL_STR("y") );
        FTL::JSONFloat64Enc<std::string> yJFE( yJE, newTopLeftPos.y() );
      }
    }
    exec.setNodeMetadata(
      nodeName.c_str(), "uiGraphPos", json.c_str(), true
      );
    ++coreUndoCount;
  }
}


void DFGUIPerform_ResizeBackDropNode(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef backDropNodeName,
  QPointF newTopLeftPos,
  QSizeF newSize,
  unsigned &coreUndoCount
  )
{
  {
    std::string json;
    {
      FTL::JSONEnc<std::string> je( json, FTL::JSONFormat::Packed() );
      FTL::JSONObjectEnc<std::string> joe( je );
      {
        FTL::JSONEnc<std::string> xJE( joe, FTL_STR("x") );
        FTL::JSONFloat64Enc<std::string> xJFE( xJE, newTopLeftPos.x() );
      }
      {
        FTL::JSONEnc<std::string> yJE( joe, FTL_STR("y") );
        FTL::JSONFloat64Enc<std::string> yJFE( yJE, newTopLeftPos.y() );
      }
    }
    exec.setNodeMetadata(
      backDropNodeName.c_str(), "uiGraphPos", json.c_str(), true
      );
    ++coreUndoCount;
  }

  {
    std::string json;
    {
      FTL::JSONEnc<std::string> je( json, FTL::JSONFormat::Packed() );
      FTL::JSONObjectEnc<std::string> joe( je );
      {
        FTL::JSONEnc<std::string> wJE( joe, FTL_STR("w") );
        FTL::JSONFloat64Enc<std::string> wJFE( wJE, newSize.width() );
      }
      {
        FTL::JSONEnc<std::string> hJE( joe, FTL_STR("h") );
        FTL::JSONFloat64Enc<std::string> hJFE( hJE, newSize.height() );
      }
    }
    exec.setNodeMetadata(
      backDropNodeName.c_str(), "uiGraphSize", json.c_str(), true
      );
    ++coreUndoCount;
  }
}

std::string DFGUIPerform_ImplodeNodes(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef desiredNodeName,
  FTL::ArrayRef<std::string> nodeNames,
  unsigned &coreUndoCount
  )
{
  size_t count = nodeNames.size();

  QPointF uiGraphPos;
  if ( count > 0 )
  {
    for ( size_t i = 0; i < count; ++i )
      uiGraphPos += GetNodeUIGraphPos( exec, nodeNames[i] );
    uiGraphPos /= count;
  }

  char const *nodeNameCStrs[count];
  for ( size_t i = 0; i < count; ++i )
    nodeNameCStrs[i] = nodeNames[i].c_str();

  std::string newNodeName =
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
    DFGUIPerform_MoveNodes(
      binding,
      execPath,
      exec,
      newNodeName,
      uiGraphPos,
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
  QPointF oldTopLeftPos = GetNodeUIGraphPos( exec, nodeName );

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

  if ( count > 0 )
  {
    std::vector<QPointF> newTopLeftPoss;
    newTopLeftPoss.reserve( newNodeNames.size() );
    for ( size_t i = 0; i < count; ++i )
      newTopLeftPoss.push_back(
        GetNodeUIGraphPos( exec, newNodeNames[i] )
        );

    QPointF avgTopLeftPos;
    for ( size_t i = 0; i < count; ++i )
      avgTopLeftPos += newTopLeftPoss[i];
    avgTopLeftPos /= count;

    QPointF delta = oldTopLeftPos - avgTopLeftPos;

    for ( size_t i = 0; i < count; ++i )
      newTopLeftPoss[i] += delta;

    DFGUIPerform_MoveNodes(
      binding,
      execPath,
      exec,
      newNodeNames,
      newTopLeftPoss,
      coreUndoCount
      );
  }

  return newNodeNames;
}

std::string DFGUIPerform_AddBackDrop(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef title,
  QPointF pos,
  unsigned &coreUndoCount
  )
{
  std::string name = exec.addUser( title.c_str() );
  ++coreUndoCount;

  exec.setNodeMetadata(
    name.c_str(),
    "uiTitle",
    title.c_str(),
    true
    );
  ++coreUndoCount;

  std::string posJSONString;
  {
    FTL::JSONEnc<std::string> je( posJSONString, FTL::JSONFormat::Packed() );
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
  exec.setNodeMetadata(
    name.c_str(),
    "uiGraphPos",
    posJSONString.c_str(),
    true
    );
  ++coreUndoCount;

  return name;
}

void DFGUIPerform_SetNodeTitle(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  FTL::CStrRef newTitle,
  unsigned &coreUndoCount
  )
{
  switch ( exec.getNodeType( nodeName.c_str() ) )
  {
    case FabricCore::DFGNodeType_User:
    {
      exec.setNodeMetadata(
        nodeName.c_str(),
        "uiTitle",
        newTitle.c_str(),
        true
        );
      ++coreUndoCount;
    }
    break;

    case FabricCore::DFGNodeType_Inst:
    {
      exec.setInstTitle(
        nodeName.c_str(),
        newTitle.c_str()
        );
      ++coreUndoCount;
    }
    break;

    default: break;
  }
}

void DFGUIPerform_SetNodeComment(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  FTL::CStrRef comment,
  unsigned &coreUndoCount
  )
{
  exec.setNodeMetadata(
    nodeName.c_str(),
    "uiComment",
    comment.c_str(),
    true
    );
  ++coreUndoCount;

  exec.setNodeMetadata(
    nodeName.c_str(),
    "uiCommentExpanded",
    !comment.empty()? "true": "",
    true
    );
  ++coreUndoCount;
}

void DFGUIPerform_SetNodeCommentExpanded(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef nodeName,
  bool expanded,
  unsigned &coreUndoCount
  )
{
  exec.setNodeMetadata(
    nodeName.c_str(),
    "uiCommentExpanded",
    expanded? "true": "",
    true
    );
  ++coreUndoCount;
}

void DFGUIPerform_SetCode(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef code,
  unsigned &coreUndoCount
  )
{
  exec.setCode( code.c_str() );
  ++coreUndoCount;
}

std::string DFGUIPerform_RenameExecPort(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef execPath,
  FabricCore::DFGExec &exec,
  FTL::CStrRef oldName,
  FTL::CStrRef desiredNewName,
  unsigned &coreUndoCount
  )
{
  FTL::CStrRef actualNewName =
    exec.renameExecPort( oldName.c_str(), desiredNewName.c_str() );
  ++coreUndoCount;
  return actualNewName;
}

FABRIC_UI_DFG_NAMESPACE_END
