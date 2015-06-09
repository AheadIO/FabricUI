// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NotificationRouter.h"
#include "DFGGraph.h"
#include "DFGController.h"

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

void NotificationRouter::callback( FTL::StrRef jsonStr )
{
  try
  {
    onNotification(jsonStr);

    FabricCore::Variant notificationVar = FabricCore::Variant::CreateFromJSON(jsonStr.data(), jsonStr.size());
    const FabricCore::Variant * descVar = notificationVar.getDictValue("desc");
    std::string descStr = descVar->getStringData();

    if(descStr == "nodeInserted")
    {
      const FabricCore::Variant * nodePathVar = notificationVar.getDictValue("path");
      char const * nodePath = nodePathVar->getStringData();
      onNodeInserted(m_coreDFGExec, nodePath);
    }
    else if(descStr == "nodeRemoved")
    {
      const FabricCore::Variant * nodePathVar = notificationVar.getDictValue("path");
      char const * nodePath = nodePathVar->getStringData();
      onNodeRemoved(m_coreDFGExec, nodePath);
    }
    else if(descStr == "nodePortInserted")
    {
      const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
      onNodePortInserted(m_coreDFGExec, nodePortPathVar->getStringData());
    }
    else if(descStr == "nodePortRemoved")
    {
      const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
      onNodePortRemoved(m_coreDFGExec, nodePortPathVar->getStringData());
    }
    else if(descStr == "execPortInserted")
    {
      const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
      onExecPortInserted(m_coreDFGExec, nodePortPathVar->getStringData());
    }
    else if(descStr == "execPortRemoved")
    {
      const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
      onExecPortRemoved(m_coreDFGExec, nodePortPathVar->getStringData());
    }
    else if(descStr == "portsConnected")
    {
      const FabricCore::Variant * srcPortPathVar = notificationVar.getDictValue("srcPath");
      const FabricCore::Variant * dstPortPathVar = notificationVar.getDictValue("dstPath");
      onPortsConnected(m_coreDFGExec, srcPortPathVar->getStringData(), dstPortPathVar->getStringData());
    }
    else if(descStr == "portsDisconnected")
    {
      const FabricCore::Variant * srcPortPathVar = notificationVar.getDictValue("srcPath");
      const FabricCore::Variant * dstPortPathVar = notificationVar.getDictValue("dstPath");
      onPortsDisconnected(m_coreDFGExec, srcPortPathVar->getStringData(), dstPortPathVar->getStringData());
    }
    else if(descStr == "nodeMetadataChanged")
    {
      const FabricCore::Variant * nodePathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * keyVar = notificationVar.getDictValue("key");
      const FabricCore::Variant * valueVar = notificationVar.getDictValue("value");
      onNodeMetadataChanged(m_coreDFGExec, nodePathVar->getStringData(), keyVar->getStringData(), valueVar->getStringData());
    }
    else if(descStr == "nodeTitleChanged")
    {
      const FabricCore::Variant * nodePathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * titleVar = notificationVar.getDictValue("title");
      onNodeTitleChanged(m_coreDFGExec, nodePathVar->getStringData(), titleVar->getStringData());
    }
    else if(descStr == "execPortRenamed")
    {
      const FabricCore::Variant * oldPathVar = notificationVar.getDictValue("oldPath");
      const FabricCore::Variant * newPathVar = notificationVar.getDictValue("newPath");
      onExecPortRenamed(m_coreDFGExec, oldPathVar->getStringData(), newPathVar->getStringData());
    }
    else if(descStr == "nodePortRenamed")
    {
      const FabricCore::Variant * oldPathVar = notificationVar.getDictValue("oldPath");
      const FabricCore::Variant * newPathVar = notificationVar.getDictValue("newPath");
      onNodePortRenamed(m_coreDFGExec, oldPathVar->getStringData(), newPathVar->getStringData());
    }
    else if(descStr == "execMetadataChanged")
    {
      const FabricCore::Variant * keyVar = notificationVar.getDictValue("key");
      const FabricCore::Variant * valueVar = notificationVar.getDictValue("value");
      onExecMetadataChanged(m_coreDFGExec, keyVar->getStringData(), valueVar->getStringData());
    }
    else if(descStr == "extDepAdded")
    {
      const FabricCore::Variant * nameVar = notificationVar.getDictValue("name");
      const FabricCore::Variant * versionRangeVar = notificationVar.getDictValue("versionRange");
      onExtDepAdded(m_coreDFGExec, nameVar->getStringData(), versionRangeVar->getStringData());
    }
    else if(descStr == "extDepRemoved")
    {
      const FabricCore::Variant * nameVar = notificationVar.getDictValue("name");
      const FabricCore::Variant * versionRangeVar = notificationVar.getDictValue("versionRange");
      onExtDepRemoved(m_coreDFGExec, nameVar->getStringData(), versionRangeVar->getStringData());
    }
    else if(descStr == "nodeCacheRuleChanged")
    {
      const FabricCore::Variant * nodePathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * cacheRuleVar = notificationVar.getDictValue("cacheRule");
      onNodeCacheRuleChanged(m_coreDFGExec, nodePathVar->getStringData(), cacheRuleVar->getStringData());
    }
    else if(descStr == "execCacheRuleChanged")
    {
      // const FabricCore::Variant * execPathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * cacheRuleVar = notificationVar.getDictValue("cacheRule");
      onExecCacheRuleChanged(m_coreDFGExec, cacheRuleVar->getStringData());
    }
    else if(descStr == "portResolvedTypeChanged")
    {
      const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * resolvedTypeVar = notificationVar.getDictValue("resolvedType");
      FTL::StrRef resolvedType;
      if(resolvedTypeVar->isString())
        resolvedType = resolvedTypeVar->getStringData();
      onExecPortResolvedTypeChanged(m_coreDFGExec, portPathVar->getStringData(), resolvedType);
    }
    else if(descStr == "portTypeSpecChanged")
    {
      const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * typeSpecVar = notificationVar.getDictValue("typeSpec");
      FTL::StrRef typeSpec;
      if(typeSpecVar->isString())
        typeSpec = typeSpecVar->getStringData();
      onExecPortTypeSpecChanged(m_coreDFGExec, portPathVar->getStringData(), typeSpec);
    }
    else if(descStr == "nodePortResolvedTypeChanged")
    {
      const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * resolvedTypeVar = notificationVar.getDictValue("resolvedType");
      FTL::StrRef resolvedType;
      if(resolvedTypeVar->isString())
        resolvedType = resolvedTypeVar->getStringData();
      onNodePortResolvedTypeChanged(m_coreDFGExec, portPathVar->getStringData(), resolvedType);
    }
    else if(descStr == "portMetadataChanged")
    {
      const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * keyVar = notificationVar.getDictValue("key");
      const FabricCore::Variant * valueVar = notificationVar.getDictValue("value");
      FTL::StrRef value;
      if(valueVar->isString())
        value = valueVar->getStringData();
      onExecPortMetadataChanged(m_coreDFGExec, portPathVar->getStringData(), keyVar->getStringData(), value);
    }
    else if(descStr == "nodePortMetadataChanged")
    {
      const FabricCore::Variant * portPathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * keyVar = notificationVar.getDictValue("key");
      const FabricCore::Variant * valueVar = notificationVar.getDictValue("value");
      FTL::StrRef value;
      if(valueVar->isString())
        value = valueVar->getStringData();
      onNodePortMetadataChanged(m_coreDFGExec, portPathVar->getStringData(), keyVar->getStringData(), value);
    }
    else if(descStr == "execPortOutsidePortTypeChanged")
    {
      const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * newPortTypeVar = notificationVar.getDictValue("newPortType");
      FTL::StrRef newPortType;
      if(newPortTypeVar->isString())
        newPortType = newPortTypeVar->getStringData();
      onExecPortTypeChanged(m_coreDFGExec, nodePortPathVar->getStringData(), newPortType);
    }
    else if(descStr == "nodePortTypeChanged")
    {
      const FabricCore::Variant * nodePortPathVar = notificationVar.getDictValue("path");
      const FabricCore::Variant * newPortTypeVar = notificationVar.getDictValue("newPortType");
      FTL::StrRef newPortType;
      if(newPortTypeVar->isString())
        newPortType = newPortTypeVar->getStringData();
      onNodePortTypeChanged(m_coreDFGExec, nodePortPathVar->getStringData(), newPortType);
    }
    else
    {
      printf("View::callback: Unhandled desc '%s', '%s'\n", descStr.c_str(), jsonStr.data());
    }
  }
  catch ( FabricCore::Exception e )
  {
    printf(
      "NotificationRouter::callback: caught Core exception: %s\n",
      e.getDesc_cstr()
      );
  }
}
