//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "DFGExecNotifier.h"

#include <FTL/JSONValue.h>
#include <stdio.h>

namespace FabricUI {
namespace DFG {

DFGExecNotifier::DFGExecNotifier( FabricCore::DFGExec exec )
  : m_view( exec.createView( &ViewCallback, this ) )
  , m_depth( 0 )
{
}

DFGExecNotifier::~DFGExecNotifier()
{
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
    if ( descStr == FTL_STR("nodeInserted") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );

      emit nodeInserted( nodeName );
    }
    else if ( descStr == FTL_STR("nodeRenamed") )
    {
      FTL::CStrRef oldNodeName = jsonObject->getString( FTL_STR("oldNodeName") );
      FTL::CStrRef newNodeName = jsonObject->getString( FTL_STR("newNodeName") );

      emit nodeRenamed( oldNodeName, newNodeName );
    }
    else if ( descStr == FTL_STR("nodeRemoved") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );

      emit nodeRemoved( nodeName );
    }
    else if ( descStr == FTL_STR("nodePortInserted") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
      FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

      emit nodePortInserted( nodeName, portIndex, portName );
    }
    else if ( descStr == FTL_STR("nodePortRenamed") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
      FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
      FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("newPortName") );

      emit nodePortRenamed( nodeName, portIndex, oldPortName, newPortName );
    }
    else if ( descStr == FTL_STR("nodePortRemoved") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
      FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

      emit nodePortRemoved( nodeName, portIndex, portName );
    }
    else if ( descStr == FTL_STR("execPortInserted") )
    {
      FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
      unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );

      emit portInserted( portIndex, portName );
    }
    else if ( descStr == FTL_STR("execPortRenamed") )
    {
      unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
      FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
      FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("newPortName") );

      emit portRenamed( portIndex, oldPortName, newPortName );
    }
    else if ( descStr == FTL_STR("execPortRemoved") )
    {
      FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
      unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );

      emit portRemoved( portIndex, portName );
    }
    else if ( descStr == FTL_STR("execPortTypeSpecChanged") )
    {
      FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
      FTL::CStrRef newTypeSpec = jsonObject->getString( FTL_STR("newTypeSpec") );

      emit portTypeSpecChanged( portName, newTypeSpec );
    }
    else if ( descStr == FTL_STR("nodePortResolvedTypeChanged") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
      FTL::CStrRef newResolvedTypeName = jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") );

      emit nodePortResolvedTypeChanged( nodeName, portName, newResolvedTypeName );
    }
    else if ( descStr == FTL_STR("nodePortDefaultValuesChanged") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );

      emit nodePortDefaultValuesChanged( nodeName, portName );
    }
    else if ( descStr == FTL_STR("execPortResolvedTypeChanged") )
    {
      FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
      FTL::CStrRef newResolvedTypeName = jsonObject->getStringOrEmpty( FTL_STR("newResolvedType") );

      emit portResolvedTypeChanged( portName, newResolvedTypeName );
    }
    else if ( descStr == FTL_STR("execEditWouldSplitFromPresetMayHaveChanged") )
    {
      emit editWouldSplitFromPresetMayHaveChanged();
    }
    else if ( descStr == FTL_STR("instExecEditWouldSplitFromPresetMayHaveChanged") )
    {
      FTL::CStrRef instName = jsonObject->getString( FTL_STR("instName") );

      emit instExecEditWouldSplitFromPresetMayHaveChanged( instName );
    }
    else if ( descStr == FTL_STR("execMetadataChanged") )
    {
      FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
      FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

      emit metadataChanged( key, value );
    }
    else if ( descStr == FTL_STR("nodeMetadataChanged") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
      FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

      emit nodeMetadataChanged( nodeName, key, value );
    }
    else if ( descStr == FTL_STR("execPortMetadataChanged") )
    {
      FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
      FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
      FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

      emit portMetadataChanged( portName, key, value );
    }
    else if ( descStr == FTL_STR("nodePortMetadataChanged") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      FTL::CStrRef portName = jsonObject->getString( FTL_STR("portName") );
      FTL::CStrRef key = jsonObject->getString( FTL_STR("key") );
      FTL::CStrRef value = jsonObject->getString( FTL_STR("value") );

      emit nodePortMetadataChanged( nodeName, portName, key, value );
    }
    else if ( descStr == FTL_STR("portsConnected") )
    {
      FTL::CStrRef srcPortPath = jsonObject->getString( FTL_STR("srcPath") );
      FTL::CStrRef dstPortPath = jsonObject->getString( FTL_STR("dstPath") );

      emit portsConnected( srcPortPath, dstPortPath );
    }
    else if ( descStr == FTL_STR("portsDisconnected") )
    {
      FTL::CStrRef srcPortPath = jsonObject->getString( FTL_STR("srcPath") );
      FTL::CStrRef dstPortPath = jsonObject->getString( FTL_STR("dstPath") );

      emit portsDisconnected( srcPortPath, dstPortPath );
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

void DFGExecNotifier::viewCallback( FTL::CStrRef jsonStr )
{
  if ( m_depth > 0 )
  {
    // Process later as we are already processing notifications
    m_queued.push_back( jsonStr );
    return;
  }

  DepthBracket _( this );

  handle( jsonStr );

  while ( !m_queued.empty() )
  {
    std::vector<std::string> queued;
    m_queued.swap( queued );
    for ( std::vector<std::string>::const_iterator it = queued.begin();
      it != queued.end(); ++it )
    {
      FTL::CStrRef queuedJSONStr = *it;
      handle( queuedJSONStr );
    }
  }
}

void DFGExecNotifier::ViewCallback(
  void * userData,
  char const *jsonCString,
  uint32_t jsonLength
  )
{
  static_cast<DFGExecNotifier *>(
    userData
    )->viewCallback(
      FTL::CStrRef( jsonCString, jsonLength )
      );
}

} // namespace DFG
} // namespace FabricUI
