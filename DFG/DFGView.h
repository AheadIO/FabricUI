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

      DFGView(FabricServices::DFGWrapper::GraphExecutable graph, const DFGConfig & config = DFGConfig());

      DFGController * getController();
      void setController(DFGController * view);

      GraphView::Port * getLastPortInserted();

      static float getFloatFromVariant(const FabricCore::Variant * variant);

    protected:

      virtual void onGraphSet();
      
      virtual void onNotification(char const * json);
      virtual void onNodeInserted(FabricServices::DFGWrapper::Node node);
      virtual void onNodeRemoved(FabricServices::DFGWrapper::Node node);
      virtual void onPinInserted(FabricServices::DFGWrapper::Pin pin);
      virtual void onPinRemoved(FabricServices::DFGWrapper::Pin pin);
      virtual void onPortInserted(FabricServices::DFGWrapper::Port pin);
      virtual void onPortRemoved(FabricServices::DFGWrapper::Port pin);
      virtual void onEndPointsConnected(FabricServices::DFGWrapper::Port src, FabricServices::DFGWrapper::Port dst);
      virtual void onEndPointsDisconnected(FabricServices::DFGWrapper::Port src, FabricServices::DFGWrapper::Port dst);
      virtual void onNodeMetadataChanged(FabricServices::DFGWrapper::Node node, const char * key, const char * metadata);
      virtual void onNodeTitleChanged(FabricServices::DFGWrapper::Node node, const char * title);
      virtual void onPortRenamed(FabricServices::DFGWrapper::Port port, const char * oldName);
      virtual void onPinRenamed(FabricServices::DFGWrapper::Pin pin, const char * oldName);
      virtual void onExecMetadataChanged(FabricServices::DFGWrapper::Executable exec, const char * key, const char * metadata);
      virtual void onExtDepAdded(const char * extension, const char * version);
      virtual void onNodeCacheRuleChanged(const char * path, const char * rule);
      virtual void onExecCacheRuleChanged(const char * path, const char * rule);

      void updateDataTypesOnPorts();

      DFGController * m_controller;
      DFGConfig m_config;
      GraphView::Port * m_lastPortInserted;
      FabricServices::DFGWrapper::GraphExecutable m_graph;
      bool m_performChecks;
    };

  };

};

#endif // __UI_DFG_DFGView__
