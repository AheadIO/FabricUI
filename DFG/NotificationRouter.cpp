// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NotificationRouter.h"
#include "DFGGraph.h"
#include "DFGController.h"

#include <FTL/JSONValue.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

NotificationRouter::NotificationRouter(
  FabricCore::DFGBinding coreDFGBinding,
  FabricCore::DFGExec coreDFGExec
  )
  : m_coreDFGBinding( coreDFGBinding )
  , m_coreDFGExec( coreDFGExec )
  , m_coreDFGView(
    coreDFGExec.createView( &Callback, this )
    )
  , m_controller(NULL)
{
}

void NotificationRouter::callback( FTL::CStrRef jsonStr )
{
  try
  {
    printf( "notif = %s\n", jsonStr.c_str() );
    
    // FabricCore::DFGStringResult desc = m_coreDFGExec.getDesc();
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
        jsonObject->getString( FTL_STR("path") ),
        jsonObject->getString( FTL_STR("key") ),
        jsonObject->getString( FTL_STR("value") )
        );
    }
    else if(descStr == FTL_STR("nodeTitleChanged"))
    {
      onNodeTitleChanged(
        jsonObject->getString( FTL_STR("path") ),
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
        m_coreDFGExec,
        jsonObject->getString( FTL_STR("oldPath") ),
        jsonObject->getString( FTL_STR("newPath") )
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
        m_coreDFGExec,
        jsonObject->getString( FTL_STR("name") ),
        jsonObject->getString( FTL_STR("versionRange") )
        );
    }
    else if(descStr == FTL_STR("extDepRemoved"))
    {
      onExtDepRemoved(
        m_coreDFGExec,
        jsonObject->getString( FTL_STR("name") ),
        jsonObject->getString( FTL_STR("versionRange") )
        );
    }
    else if(descStr == FTL_STR("nodeCacheRuleChanged"))
    {
      onNodeCacheRuleChanged(
        m_coreDFGExec,
        jsonObject->getString( FTL_STR("path") ),
        jsonObject->getString( FTL_STR("cacheRule") )
        );
    }
    else if(descStr == FTL_STR("execCacheRuleChanged"))
    {
      onExecCacheRuleChanged(
        m_coreDFGExec,
        jsonObject->getString( FTL_STR("cacheRule") )
        );
    }
    else if(descStr == FTL_STR("portResolvedTypeChanged"))
    {
      onExecPortResolvedTypeChanged(
        m_coreDFGExec,
        jsonObject->getString( FTL_STR("path") ),
        jsonObject->getStringOrEmpty( FTL_STR("resolvedType") )
        );
    }
    else if(descStr == FTL_STR("portTypeSpecChanged"))
    {
      onExecPortTypeSpecChanged(
        m_coreDFGExec,
        jsonObject->getString( FTL_STR("path") ),
        jsonObject->getStringOrEmpty( FTL_STR("typeSpec") )
        );
    }
    else if(descStr == FTL_STR("nodePortResolvedTypeChanged"))
    {
      onNodePortResolvedTypeChanged(
        m_coreDFGExec,
        jsonObject->getString( FTL_STR("path") ),
        jsonObject->getStringOrEmpty( FTL_STR("resolvedType") )
        );
    }
    else if(descStr == FTL_STR("portMetadataChanged"))
    {
      onExecPortMetadataChanged(
        jsonObject->getString( FTL_STR("path") ),
        jsonObject->getString( FTL_STR("key") ),
        jsonObject->getString( FTL_STR("value") )
        );
    }
    else if(descStr == FTL_STR("nodePortMetadataChanged"))
    {
      onNodePortMetadataChanged(
        jsonObject->getString( FTL_STR("path") ),
        jsonObject->getString( FTL_STR("key") ),
        jsonObject->getString( FTL_STR("value") )
        );
    }
    else if(descStr == FTL_STR("execPortOutsidePortTypeChanged"))
    {
      onExecPortTypeChanged(
        jsonObject->getString( FTL_STR("path") ),
        jsonObject->getString( FTL_STR("newPortType") )
        );
    }
    else if(descStr == FTL_STR("nodePortTypeChanged"))
    {
      onNodePortTypeChanged(
        jsonObject->getString( FTL_STR("path") ),
        jsonObject->getString( FTL_STR("newPortType") )
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
