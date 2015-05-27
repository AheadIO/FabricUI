// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGView__
#define __UI_DFG_DFGView__

#include <DFGWrapper/View.h>
#include <GraphView/Controller.h>
#include <GraphView/Port.h>
#include "DFGConfig.h"

namespace FabricUI
{

  namespace DFG
  {
    // forward decl
    class DFGController;

    class DFGView : public FabricServices::DFGWrapper::View
    {

      friend class DFGController;
      friend class DFGWidget;

    public:

      DFGView(FabricServices::DFGWrapper::GraphExecutablePtr graph, const DFGConfig & config = DFGConfig());

      DFGController * getController();
      void setController(DFGController * view);
      FabricServices::DFGWrapper::GraphExecutablePtr getGraph();

      GraphView::Port * getLastPortInserted();

      static float getFloatFromVariant(const FabricCore::Variant * variant);

    protected:

      virtual void onGraphSet();
      
      virtual void onNotification(char const * json);
      virtual void onNodeInserted(FabricServices::DFGWrapper::NodePtr node);
      virtual void onNodeRemoved(FabricServices::DFGWrapper::NodePtr node);
      virtual void onPinInserted(FabricServices::DFGWrapper::PinPtr pin);
      virtual void onPinRemoved(FabricServices::DFGWrapper::PinPtr pin);
      virtual void onExecPortInserted(FabricServices::DFGWrapper::ExecPortPtr pin);
      virtual void onExecPortRemoved(FabricServices::DFGWrapper::ExecPortPtr pin);
      virtual void onEndPointsConnected(FabricServices::DFGWrapper::EndPointPtr src, FabricServices::DFGWrapper::EndPointPtr dst);
      virtual void onEndPointsDisconnected(FabricServices::DFGWrapper::EndPointPtr src, FabricServices::DFGWrapper::EndPointPtr dst);
      virtual void onNodeMetadataChanged(FabricServices::DFGWrapper::NodePtr node, const char * key, const char * metadata);
      virtual void onNodeTitleChanged(FabricServices::DFGWrapper::NodePtr node, const char * title);
      virtual void onExecPortRenamed(FabricServices::DFGWrapper::ExecPortPtr port, const char * oldName);
      virtual void onPinRenamed(FabricServices::DFGWrapper::PinPtr pin, const char * oldName);
      virtual void onExecMetadataChanged(FabricServices::DFGWrapper::ExecutablePtr exec, const char * key, const char * metadata);
      virtual void onExtDepAdded(const char * extension, const char * version);
      virtual void onExtDepRemoved(const char * extension, const char * version);
      virtual void onNodeCacheRuleChanged(const char * path, const char * rule);
      virtual void onExecCacheRuleChanged(const char * path, const char * rule);
      virtual void onExecPortResolvedTypeChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * resolvedType);
      virtual void onExecPortTypeSpecChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * typeSpec);
      virtual void onPinResolvedTypeChanged(FabricServices::DFGWrapper::PinPtr pin, const char * resolvedType);
      virtual void onExecPortMetadataChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * key, const char * metadata);
      virtual void onPinMetadataChanged(FabricServices::DFGWrapper::PinPtr pin, const char * key, const char * metadata);
      virtual void onPinTypeChanged(FabricServices::DFGWrapper::PinPtr pin, FabricCore::DFGPortType pinType);
      virtual void onExecPortTypeChanged(FabricServices::DFGWrapper::ExecPortPtr port, FabricCore::DFGPortType portType);

      DFGController * m_controller;
      DFGConfig m_config;
      GraphView::Port * m_lastPortInserted;
      FabricServices::DFGWrapper::GraphExecutablePtr m_graph;
      bool m_performChecks;
    };

  };

};

#endif // __UI_DFG_DFGView__
