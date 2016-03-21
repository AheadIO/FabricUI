//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "DFGExecNotifier.h"

#include <FTL/JSONValue.h>
#include <stdio.h>

namespace FabricUI {
namespace DFG {

DFGExecNotifier::HandlerMap const &DFGExecNotifier::GetHandlerMap()
{
  static HandlerMap handlerMap;
  if ( handlerMap.empty() )
  {
    handlerMap[FTL_STR("nodeInserted")] = &DFGExecNotifier::handler_nodeInserted;
    handlerMap[FTL_STR("nodeRenamed")] = &DFGExecNotifier::handler_nodeRenamed;
    handlerMap[FTL_STR("nodeRemoved")] = &DFGExecNotifier::handler_nodeRemoved;
    handlerMap[FTL_STR("nodePortInserted")] = &DFGExecNotifier::handler_nodePortInserted;
    handlerMap[FTL_STR("nodePortRenamed")] = &DFGExecNotifier::handler_nodePortRenamed;
    handlerMap[FTL_STR("nodePortRemoved")] = &DFGExecNotifier::handler_nodePortRemoved;
    handlerMap[FTL_STR("execPortInserted")] = &DFGExecNotifier::handler_execPortInserted;
    handlerMap[FTL_STR("execPortRenamed")] = &DFGExecNotifier::handler_execPortRenamed;
    handlerMap[FTL_STR("execPortRemoved")] = &DFGExecNotifier::handler_execPortRemoved;
    handlerMap[FTL_STR("execPortTypeSpecChanged")] = &DFGExecNotifier::handler_execPortTypeSpecChanged;
    handlerMap[FTL_STR("nodePortResolvedTypeChanged")] = &DFGExecNotifier::handler_nodePortResolvedTypeChanged;
    handlerMap[FTL_STR("nodePortDefaultValuesChanged")] = &DFGExecNotifier::handler_nodePortDefaultValuesChanged;
    handlerMap[FTL_STR("execPortResolvedTypeChanged")] = &DFGExecNotifier::handler_execPortResolvedTypeChanged;
    handlerMap[FTL_STR("execPortsReordered")] = &DFGExecNotifier::handler_execPortsReordered;
    handlerMap[FTL_STR("nodePortsReordered")] = &DFGExecNotifier::handler_nodePortsReordered;
    handlerMap[FTL_STR("execEditWouldSplitFromPresetMayHaveChanged")] = &DFGExecNotifier::handler_execEditWouldSplitFromPresetMayHaveChanged;
    handlerMap[FTL_STR("instExecEditWouldSplitFromPresetMayHaveChanged")] = &DFGExecNotifier::handler_instExecEditWouldSplitFromPresetMayHaveChanged;
    handlerMap[FTL_STR("execMetadataChanged")] = &DFGExecNotifier::handler_execMetadataChanged;
    handlerMap[FTL_STR("execTitleChanged")] = &DFGExecNotifier::handler_execTitleChanged;
    handlerMap[FTL_STR("execPortDefaultValuesChanged")] = &DFGExecNotifier::handler_execPortDefaultValuesChanged;
    handlerMap[FTL_STR("instExecTitleChanged")] = &DFGExecNotifier::handler_instExecTitleChanged;
    handlerMap[FTL_STR("nodeMetadataChanged")] = &DFGExecNotifier::handler_nodeMetadataChanged;
    handlerMap[FTL_STR("execPortMetadataChanged")] = &DFGExecNotifier::handler_execPortMetadataChanged;
    handlerMap[FTL_STR("nodePortMetadataChanged")] = &DFGExecNotifier::handler_nodePortMetadataChanged;
    handlerMap[FTL_STR("portsConnected")] = &DFGExecNotifier::handler_portsConnected;
    handlerMap[FTL_STR("portsDisconnected")] = &DFGExecNotifier::handler_portsDisconnected;
    handlerMap[FTL_STR("refVarPathChanged")] = &DFGExecNotifier::handler_refVarPathChanged;
    handlerMap[FTL_STR("funcCodeChanged")] = &DFGExecNotifier::handler_funcCodeChanged;
    handlerMap[FTL_STR("nodePortTypeChanged")] = &DFGExecNotifier::handler_nodePortTypeChanged;
    handlerMap[FTL_STR("execPortTypeChanged")] = &DFGExecNotifier::handler_execPortTypeChanged;
    handlerMap[FTL_STR("instExecDidAttachPreset")] = &DFGExecNotifier::handler_instExecDidAttachPreset;
    handlerMap[FTL_STR("execDidAttachPreset")] = &DFGExecNotifier::handler_execDidAttachPreset;
    handlerMap[FTL_STR("execWillDetachPreset")] = &DFGExecNotifier::handler_execWillDetachPreset;
    handlerMap[FTL_STR("extDepsChanged")] = &DFGExecNotifier::handler_extDepsChanged;
  }
  return handlerMap;
}

DFGExecNotifier::~DFGExecNotifier()
{
  try
  {
    m_view.invalidate();
  }
  catch ( FabricCore::Exception e )
  {
    // [andrew 20160320] an exception will be thrown here if the Core
    // is destroyed before the Qt widgets
  }
}

void DFGExecNotifier::handle( FTL::CStrRef jsonStr )
{
  try
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );
    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
      );
    FTL::CStrRef descStr = jsonObject->getString( FTL_STR("desc") );

    HandlerMap const &handlerMap = GetHandlerMap();
    HandlerMap::const_iterator it = handlerMap.find( descStr );
    if ( it != handlerMap.end() )
    {
      Handler handler = it->second;
      (this->*handler)( jsonObject.get() );
    }
    else
    {
      fprintf(
        stderr,
        "DFGExecNotifier::viewCallback: warning: unhandled notification:\n%s\n",
        jsonStr.c_str()
        );
    }
  }
  catch ( FabricCore::Exception e )
  {
    printf(
      "DFGExecNotifier::viewCallback: caught Core exception: %s\n",
      e.getDesc_cstr()
      );
  }
  catch ( FTL::JSONException e )
  {
    printf(
      "DFGExecNotifier::viewCallback: caught FTL::JSONException: %s\n",
      e.getDescCStr()
      );
  }
}

void DFGExecNotifier::handler_nodeInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );

  emit nodeInserted( nodeName );
}

void DFGExecNotifier::handler_nodeRenamed( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef oldNodeName = jsonObject->getString( FTL_STR("oldNodeName") );
  FTL::CStrRef newNodeName = jsonObject->getString( FTL_STR("newNodeName") );

  emit nodeRenamed( oldNodeName, newNodeName );
}

void DFGExecNotifier::handler_nodeRemoved( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );

  emit nodeRemoved( nodeName );
}

void DFGExecNotifier::handler_nodePortInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit nodePortInserted( nodeName, portIndex, portName );
}

void DFGExecNotifier::handler_nodePortRenamed( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
  FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("newPortName") );

  emit nodePortRenamed( nodeName, portIndex, oldPortName, newPortName );
}

void DFGExecNotifier::handler_nodePortRemoved( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit nodePortRemoved( nodeName, portIndex, portName );
}

void DFGExecNotifier::handler_execPortInserted( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );

  emit portInserted( portIndex, portName );
}

void DFGExecNotifier::handler_execPortRenamed( FTL::JSONObject const *jsonObject )
{
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
  FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
  FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("newPortName") );

  emit portRenamed( portIndex, oldPortName, newPortName );
}

void DFGExecNotifier::handler_execPortRemoved( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );

  emit portRemoved( portIndex, portName );
}

void DFGExecNotifier::handler_execPortTypeSpecChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newTypeSpec = jsonObject->getString( FTL_STR("newTypeSpec") );

  emit portTypeSpecChanged( portName, newTypeSpec );
}

void DFGExecNotifier::handler_execPortsReordered( FTL::JSONObject const *jsonObject )
{
  FTL::JSONArray const *newOrderJSONArray =
    jsonObject->getArray( FTL_STR("newOrder") );
  size_t newOrderSize = newOrderJSONArray->size();
  unsigned *newOrderData =
    (unsigned *)alloca( newOrderSize * sizeof( unsigned ) );
  for ( size_t i = 0; i < newOrderSize; ++i )
    newOrderData[i] = unsigned( newOrderJSONArray->getSInt32( i ) );

  emit portsReordered(
    FTL::ArrayRef<unsigned>( newOrderData, newOrderSize )
    );
}

void DFGExecNotifier::handler_nodePortsReordered( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::JSONArray const *newOrderJSONArray =
    jsonObject->getArray( FTL_STR("newOrder") );
  size_t newOrderSize = newOrderJSONArray->size();
  unsigned *newOrderData =
    (unsigned *)alloca( newOrderSize * sizeof( unsigned ) );
  for ( size_t i = 0; i < newOrderSize; ++i )
    newOrderData[i] = unsigned( newOrderJSONArray->getSInt32( i ) );

  emit nodePortsReordered(
    nodeName,
    FTL::ArrayRef<unsigned>( newOrderData, newOrderSize )
    );
}

void DFGExecNotifier::handler_nodePortResolvedTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newResolvedTypeName = jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") );

  emit nodePortResolvedTypeChanged( nodeName, portName, newResolvedTypeName );
}

void DFGExecNotifier::handler_nodePortDefaultValuesChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

  emit nodePortDefaultValuesChanged( nodeName, portName );
}

void DFGExecNotifier::handler_execPortResolvedTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newResolvedTypeName = jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") );

  emit portResolvedTypeChanged( portName, newResolvedTypeName );
}

void DFGExecNotifier::handler_execEditWouldSplitFromPresetMayHaveChanged( FTL::JSONObject const *jsonObject )
{
  emit editWouldSplitFromPresetMayHaveChanged();
}

void DFGExecNotifier::handler_instExecEditWouldSplitFromPresetMayHaveChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );

  emit instExecEditWouldSplitFromPresetMayHaveChanged( instName );
}

void DFGExecNotifier::handler_execMetadataChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
  FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

  emit metadataChanged( key, value );
}

void DFGExecNotifier::handler_execTitleChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef title = jsonObject->getString( FTL_STR("title") );

  emit titleChanged( title );
}

void DFGExecNotifier::handler_execPortDefaultValuesChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("execPortName") );

  emit portDefaultValuesChanged( portName );
}

void DFGExecNotifier::handler_instExecTitleChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );
  FTL::CStrRef execTitle = jsonObject->getString( FTL_STR("execTitle") );

  emit instExecTitleChanged( instName, execTitle );
}

void DFGExecNotifier::handler_nodeMetadataChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
  FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

  emit nodeMetadataChanged( nodeName, key, value );
}

void DFGExecNotifier::handler_execPortMetadataChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
  FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

  emit portMetadataChanged( portName, key, value );
}

void DFGExecNotifier::handler_nodePortMetadataChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
  FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

  emit nodePortMetadataChanged( nodeName, portName, key, value );
}

void DFGExecNotifier::handler_portsConnected( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef srcPortPath = jsonObject->getString( FTL_STR("srcPath") );
  FTL::CStrRef dstPortPath = jsonObject->getString( FTL_STR("dstPath") );

  emit portsConnected( srcPortPath, dstPortPath );
}

void DFGExecNotifier::handler_portsDisconnected( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef srcPortPath = jsonObject->getString( FTL_STR("srcPath") );
  FTL::CStrRef dstPortPath = jsonObject->getString( FTL_STR("dstPath") );

  emit portsDisconnected( srcPortPath, dstPortPath );
}

void DFGExecNotifier::handler_refVarPathChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef refName = jsonObject->getString( FTL_STR("refName") );
  FTL::CStrRef newVarPath = jsonObject->getString( FTL_STR("newVarPath") );

  emit refVarPathChanged( refName, newVarPath );
}

void DFGExecNotifier::handler_funcCodeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef code = jsonObject->getString( FTL_STR("code") );

  emit funcCodeChanged( code );
}

void DFGExecNotifier::handler_nodePortTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newNodePortType = jsonObject->getString( FTL_STR("newNodePortType") );

  emit nodePortTypeChanged( nodeName, portName, newNodePortType );
}

void DFGExecNotifier::handler_execPortTypeChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
  FTL::CStrRef newExecPortType = jsonObject->getString( FTL_STR("newExecPortType") );

  emit portTypeChanged( portName, newExecPortType );
}

void DFGExecNotifier::handler_instExecDidAttachPreset( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
  FTL::CStrRef presetFilePath = jsonObject->getString( FTL_STR("presetFilePath") );

  emit instExecDidAttachPreset( nodeName, presetFilePath );
}

void DFGExecNotifier::handler_execDidAttachPreset( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef presetFilePath = jsonObject->getString( FTL_STR("presetFilePath") );

  emit didAttachPreset( presetFilePath );
}

void DFGExecNotifier::handler_execWillDetachPreset( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef presetFilePath = jsonObject->getString( FTL_STR("presetFilePath") );

  emit willDetachPreset( presetFilePath );
}

void DFGExecNotifier::handler_extDepsChanged( FTL::JSONObject const *jsonObject )
{
  FTL::CStrRef extDeps = jsonObject->getString( FTL_STR("extDeps") );

  emit extDepsChanged( extDeps );
}

} // namespace DFG
} // namespace FabricUI
