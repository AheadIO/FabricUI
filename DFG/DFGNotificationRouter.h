// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGNotificationRouter__
#define __UI_DFG_DFGNotificationRouter__

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

      // virtual void onGraphSet();
      virtual void onNotification(FTL::StrRef json);
      virtual void onNodeInserted(FabricCore::DFGExec parent, FTL::StrRef nodePath);
      // virtual void onNodeRemoved(FabricServices::DFGWrapper::NodePtr node);
      virtual void onNodePortInserted(FabricCore::DFGExec parent, FTL::StrRef nodePortPath);
      // virtual void onNodePortRemoved(FabricServices::DFGWrapper::NodePortPtr pin);
      // virtual void onExecPortInserted(FabricServices::DFGWrapper::ExecPortPtr pin);
      // virtual void onExecPortRemoved(FabricServices::DFGWrapper::ExecPortPtr pin);
      // virtual void onPortsConnected(FabricServices::DFGWrapper::PortPtr src, FabricServices::DFGWrapper::PortPtr dst);
      // virtual void onPortsDisconnected(FabricServices::DFGWrapper::PortPtr src, FabricServices::DFGWrapper::PortPtr dst);
      virtual void onNodeMetadataChanged(FabricCore::DFGExec parent, FTL::StrRef nodePath, FTL::StrRef key, FTL::StrRef metadata);
      virtual void onNodeTitleChanged(FabricCore::DFGExec parent, FTL::StrRef nodePath, FTL::StrRef title);
      // virtual void onExecPortRenamed(FabricServices::DFGWrapper::ExecPortPtr port, const char * oldName);
      // virtual void onNodePortRenamed(FabricServices::DFGWrapper::NodePortPtr pin, const char * oldName);
      // virtual void onExecMetadataChanged(FabricServices::DFGWrapper::ExecutablePtr exec, const char * key, const char * metadata);
      // virtual void onExtDepAdded(const char * extension, const char * version);
      // virtual void onExtDepRemoved(const char * extension, const char * version);
      // virtual void onNodeCacheRuleChanged(const char * path, const char * rule);
      // virtual void onExecCacheRuleChanged(const char * path, const char * rule);
      // virtual void onExecPortResolvedTypeChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * resolvedType);
      // virtual void onExecPortTypeSpecChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * typeSpec);
      // virtual void onNodePortResolvedTypeChanged(FabricServices::DFGWrapper::NodePortPtr pin, const char * resolvedType);
      // virtual void onExecPortMetadataChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * key, const char * metadata);
      // virtual void onNodePortMetadataChanged(FabricServices::DFGWrapper::NodePortPtr pin, const char * key, const char * metadata);
      // virtual void onNodePortTypeChanged(FabricServices::DFGWrapper::NodePortPtr pin, FabricCore::DFGPortType pinType);
      // virtual void onExecPortTypeChanged(FabricServices::DFGWrapper::ExecPortPtr port, FabricCore::DFGPortType portType);

    private:

      DFGConfig m_config;
      GraphView::Port * m_lastPortInserted;
      bool m_performChecks;
    };

  };

};

#endif // __UI_DFG_DFGNotificationRouter__
