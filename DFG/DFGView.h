// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGView__
#define __UI_DFG_DFGView__

#include <FTL/StrRef.h>
#include <GraphView/Controller.h>
#include <GraphView/Port.h>
#include "DFGConfig.h"

namespace FabricUI
{

  namespace DFG
  {
    // forward decl
    class DFGController;

    class DFGView
    {

      friend class DFGController;
      friend class DFGWidget;

    public:

      DFGView(
        FabricCore::DFGExec const &coreDFGGraph,
        const DFGConfig & config = DFGConfig()
        );

      DFGController * getController();
      void setController(DFGController * view);
      
      FabricCore::DFGExec const &getCoreDFGGraph()
        { return m_coreDFGGraph; }

      GraphView::Port * getLastPortInserted();

      static float getFloatFromVariant(const FabricCore::Variant * variant);

    protected:

      // virtual void onGraphSet();
      // virtual void onNotification(char const * json);
      // virtual void onNodeInserted(FabricServices::DFGWrapper::NodePtr node);
      // virtual void onNodeRemoved(FabricServices::DFGWrapper::NodePtr node);
      // virtual void onNodePortInserted(FabricServices::DFGWrapper::NodePortPtr pin);
      // virtual void onNodePortRemoved(FabricServices::DFGWrapper::NodePortPtr pin);
      // virtual void onExecPortInserted(FabricServices::DFGWrapper::ExecPortPtr pin);
      // virtual void onExecPortRemoved(FabricServices::DFGWrapper::ExecPortPtr pin);
      // virtual void onPortsConnected(FabricServices::DFGWrapper::PortPtr src, FabricServices::DFGWrapper::PortPtr dst);
      // virtual void onPortsDisconnected(FabricServices::DFGWrapper::PortPtr src, FabricServices::DFGWrapper::PortPtr dst);
      // virtual void onNodeMetadataChanged(FabricServices::DFGWrapper::NodePtr node, const char * key, const char * metadata);
      // virtual void onNodeTitleChanged(FabricServices::DFGWrapper::NodePtr node, const char * title);
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

      void viewCallback( FTL::StrRef jsonStr );

      static void ViewCallback(
        void *thisVoidPtr,
        char const *jsonCStr,
        uint32_t jsonSize
        )
      {
        static_cast<DFGView *>( thisVoidPtr )->viewCallback(
          FTL::StrRef( jsonCStr, jsonSize )
          );
      }

      DFGController * m_controller;
      DFGConfig m_config;
      GraphView::Port * m_lastPortInserted;
      FabricCore::DFGExec m_coreDFGGraph;
      bool m_performChecks;
    };

  };

};

#endif // __UI_DFG_DFGView__
