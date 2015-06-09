// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGNotificationRouter__
#define __UI_DFG_DFGNotificationRouter__

#include <FTL/JSONValue.h>
#include <FTL/StrRef.h>
#include <GraphView/Controller.h>
#include <GraphView/Port.h>
#include "NotificationRouter.h"
#include "DFGConfig.h"

namespace FabricUI
{

  namespace DFG
  {
    // forward decl
    class DFGController;

    class DFGNotificationRouter : public NotificationRouter
    {

      friend class DFGController;
      friend class DFGWidget;

    public:

      DFGNotificationRouter(
        FabricCore::DFGBinding coreDFGBinding,
        FabricCore::DFGExec coreDFGGraph,
        const DFGConfig & config = DFGConfig()
        );

      GraphView::Port * getLastPortInserted();

      static float getFloatFromVariant(const FabricCore::Variant * variant);


    protected:

      virtual void onGraphSet();
      virtual void onNotification(FTL::StrRef json);
      virtual void onNodeInserted(FTL::JSONObject const *jsonObject);
      virtual void onNodeRemoved(FabricCore::DFGExec parent, FTL::StrRef nodePath);
      virtual void onNodePortInserted(FabricCore::DFGExec parent, FTL::StrRef nodePortPath);
      virtual void onNodePortRemoved(FabricCore::DFGExec parent, FTL::StrRef nodePortPath);
      virtual void onExecPortInserted(FabricCore::DFGExec exec, FTL::StrRef portPath);
      virtual void onExecPortRemoved(FabricCore::DFGExec exec, FTL::StrRef portPath);
      virtual void onPortsConnected(FabricCore::DFGExec exec, FTL::StrRef srcPath, FTL::StrRef dstPath);
      virtual void onPortsDisconnected(FabricCore::DFGExec exec, FTL::StrRef srcPath, FTL::StrRef dstPath);
      virtual void onNodeMetadataChanged(FTL::StrRef nodePath, FTL::StrRef key, FTL::StrRef metadata);
      virtual void onNodeTitleChanged(FabricCore::DFGExec parent, FTL::StrRef nodePath, FTL::StrRef title);
      virtual void onExecPortRenamed(FabricCore::DFGExec exec, FTL::StrRef oldPath, FTL::StrRef newPath);
      virtual void onNodePortRenamed(FabricCore::DFGExec parent, FTL::StrRef oldPath, FTL::StrRef newPath);
      virtual void onExecMetadataChanged(FabricCore::DFGExec exec, FTL::StrRef key, FTL::StrRef metadata);
      virtual void onExtDepAdded(FabricCore::DFGExec exec, FTL::StrRef extension, FTL::StrRef version);
      virtual void onExtDepRemoved(FabricCore::DFGExec exec, FTL::StrRef extension, FTL::StrRef version);
      virtual void onNodeCacheRuleChanged(FabricCore::DFGExec parent, FTL::StrRef path, FTL::StrRef rule);
      virtual void onExecCacheRuleChanged(FabricCore::DFGExec exec, FTL::StrRef rule);
      virtual void onExecPortResolvedTypeChanged(FabricCore::DFGExec exec,  FTL::StrRef portPath, FTL::StrRef resolvedType);
      virtual void onExecPortTypeSpecChanged(FabricCore::DFGExec exec,  FTL::StrRef portPath, FTL::StrRef typeSpec);
      virtual void onNodePortResolvedTypeChanged(FabricCore::DFGExec exec,  FTL::StrRef nodePortPath, FTL::StrRef resolvedType);
      virtual void onExecPortMetadataChanged(FTL::StrRef portPath, FTL::StrRef key, FTL::StrRef metadata);
      virtual void onNodePortMetadataChanged(FTL::StrRef nodePortPath, FTL::StrRef key, FTL::StrRef metadata);
      virtual void onExecPortTypeChanged(FTL::StrRef portPath, FTL::StrRef portType);
      virtual void onNodePortTypeChanged(FTL::StrRef nodePortPath, FTL::StrRef portType);

    private:

      DFGConfig m_config;
      GraphView::Port * m_lastPortInserted;
      bool m_performChecks;
    };

  };

};

#endif // __UI_DFG_DFGNotificationRouter__
