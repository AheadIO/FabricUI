// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_NotificationRouter__
#define __UI_DFG_NotificationRouter__

#include <FTL/StrRef.h>
#include <GraphView/Controller.h>
#include "DFGConfig.h"

namespace FabricUI
{

  namespace DFG
  {
    // forward decl
    class DFGController;

    class NotificationRouter
    {
    public:

      NotificationRouter(
        FabricCore::DFGBinding coreDFGBinding,
        FabricCore::DFGExec coreDFGGraph
        );

      FabricCore::DFGBinding const &getCoreDFGBinding()
        { return m_coreDFGBinding; }

      FabricCore::DFGExec const &getCoreDFGExec()
        { return m_coreDFGExec; }

      GraphView::Controller * getController()
        { return m_controller; }

      void setController(GraphView::Controller * controller)
        { m_controller = controller; }

    protected:

      virtual void onNotification(FTL::StrRef json) = 0;
      virtual void onNodeInserted(FabricCore::DFGExec parent, FTL::StrRef nodePath) = 0;
      // virtual void onNodeRemoved(FabricServices::DFGWrapper::NodePtr node) = 0;
      virtual void onNodePortInserted(FabricCore::DFGExec parent, FTL::StrRef nodePortPath) = 0;
      // virtual void onNodePortRemoved(FabricServices::DFGWrapper::NodePortPtr pin) = 0;
      // virtual void onExecPortInserted(FabricServices::DFGWrapper::ExecPortPtr pin) = 0;
      // virtual void onExecPortRemoved(FabricServices::DFGWrapper::ExecPortPtr pin) = 0;
      // virtual void onPortsConnected(FabricServices::DFGWrapper::PortPtr src, FabricServices::DFGWrapper::PortPtr dst) = 0;
      // virtual void onPortsDisconnected(FabricServices::DFGWrapper::PortPtr src, FabricServices::DFGWrapper::PortPtr dst) = 0;
      virtual void onNodeMetadataChanged(FabricCore::DFGExec parent, FTL::StrRef nodePath, FTL::StrRef key, FTL::StrRef metadata) = 0;
      virtual void onNodeTitleChanged(FabricCore::DFGExec parent, FTL::StrRef nodePath, FTL::StrRef title) = 0;
      // virtual void onExecPortRenamed(FabricServices::DFGWrapper::ExecPortPtr port, const char * oldName) = 0;
      // virtual void onNodePortRenamed(FabricServices::DFGWrapper::NodePortPtr pin, const char * oldName) = 0;
      // virtual void onExecMetadataChanged(FabricServices::DFGWrapper::ExecutablePtr exec, const char * key, const char * metadata) = 0;
      // virtual void onExtDepAdded(const char * extension, const char * version) = 0;
      // virtual void onExtDepRemoved(const char * extension, const char * version) = 0;
      // virtual void onNodeCacheRuleChanged(const char * path, const char * rule) = 0;
      // virtual void onExecCacheRuleChanged(const char * path, const char * rule) = 0;
      // virtual void onExecPortResolvedTypeChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * resolvedType) = 0;
      // virtual void onExecPortTypeSpecChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * typeSpec) = 0;
      // virtual void onNodePortResolvedTypeChanged(FabricServices::DFGWrapper::NodePortPtr pin, const char * resolvedType) = 0;
      // virtual void onExecPortMetadataChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * key, const char * metadata) = 0;
      // virtual void onNodePortMetadataChanged(FabricServices::DFGWrapper::NodePortPtr pin, const char * key, const char * metadata) = 0;
      // virtual void onNodePortTypeChanged(FabricServices::DFGWrapper::NodePortPtr pin, FabricCore::DFGPortType pinType) = 0;
      // virtual void onExecPortTypeChanged(FabricServices::DFGWrapper::ExecPortPtr port, FabricCore::DFGPortType portType) = 0;

    private:

      void callback( FTL::StrRef jsonStr );

      static void Callback(
        void *thisVoidPtr,
        char const *jsonCStr,
        uint32_t jsonSize
        )
      {
        static_cast<NotificationRouter *>( thisVoidPtr )->callback(
          FTL::StrRef( jsonCStr, jsonSize )
          );
      }

    protected:

      FabricCore::DFGBinding m_coreDFGBinding;
      FabricCore::DFGExec m_coreDFGExec;
      FabricCore::DFGView m_coreDFGView;
      GraphView::Controller * m_controller;
    };

  };

};

#endif // __UI_DFG_NotificationRouter__
