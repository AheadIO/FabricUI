//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONValue.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

std::vector<std::string> DFGUICmd_Exec::adjustNewNodes(
  FabricCore::DFGStringResult const &newNodeNamesJSON,
  QPointF targetPos,
  unsigned &coreUndoCount
  )
{
  FabricCore::DFGBinding &binding = getBinding();
  FTL::CStrRef execPath = getExecPath();
  FabricCore::DFGExec &exec = getExec();

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

    QPointF delta = targetPos - avgTopLeftPos;

    for ( size_t i = 0; i < count; ++i )
      newTopLeftPoss[i] += delta;

    MoveNodes(
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

void DFGUICmd_Exec::MoveNodes(
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
      nodeName.c_str(), "uiGraphPos", json.c_str(), true, false
      );
    ++coreUndoCount;
  }
}

QPointF DFGUICmd_Exec::GetNodeUIGraphPos( 
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

FABRIC_UI_DFG_NAMESPACE_END
