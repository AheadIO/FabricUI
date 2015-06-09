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
      virtual void onNodeRemoved(FabricCore::DFGExec parent, FTL::StrRef nodePath) = 0;
      virtual void onNodePortInserted(FabricCore::DFGExec parent, FTL::StrRef nodePortPath) = 0;
      virtual void onNodePortRemoved(FabricCore::DFGExec parent, FTL::StrRef nodePortPath) = 0;
      virtual void onExecPortInserted(FabricCore::DFGExec exec, FTL::StrRef portPath) = 0;
      virtual void onExecPortRemoved(FabricCore::DFGExec exec, FTL::StrRef portPath) = 0;
      virtual void onPortsConnected(FabricCore::DFGExec exec, FTL::StrRef srcPath, FTL::StrRef dstPath) = 0;
      virtual void onPortsDisconnected(FabricCore::DFGExec exec, FTL::StrRef srcPath, FTL::StrRef dstPath) = 0;
      virtual void onNodeMetadataChanged(FabricCore::DFGExec parent, FTL::StrRef nodePath, FTL::StrRef key, FTL::StrRef metadata) = 0;
      virtual void onNodeTitleChanged(FabricCore::DFGExec parent, FTL::StrRef nodePath, FTL::StrRef title) = 0;
      virtual void onExecPortRenamed(FabricCore::DFGExec exec, FTL::StrRef oldPath, FTL::StrRef newPath) = 0;
      virtual void onNodePortRenamed(FabricCore::DFGExec parent, FTL::StrRef oldPath, FTL::StrRef newPath) = 0;
      virtual void onExecMetadataChanged(FabricCore::DFGExec exec, FTL::StrRef key, FTL::StrRef metadata) = 0;
      virtual void onExtDepAdded(FabricCore::DFGExec exec, FTL::StrRef extension, FTL::StrRef version) = 0;
      virtual void onExtDepRemoved(FabricCore::DFGExec exec, FTL::StrRef extension, FTL::StrRef version) = 0;
      virtual void onNodeCacheRuleChanged(FabricCore::DFGExec parent, FTL::StrRef path, FTL::StrRef rule) = 0;
      virtual void onExecCacheRuleChanged(FabricCore::DFGExec exec, FTL::StrRef rule) = 0;
      virtual void onExecPortResolvedTypeChanged(FabricCore::DFGExec exec,  FTL::StrRef portPath, FTL::StrRef resolvedType) = 0;
      virtual void onExecPortTypeSpecChanged(FabricCore::DFGExec exec,  FTL::StrRef portPath, FTL::StrRef typeSpec) = 0;
      virtual void onNodePortResolvedTypeChanged(FabricCore::DFGExec exec,  FTL::StrRef nodePortPath, FTL::StrRef resolvedType) = 0;
      virtual void onExecPortMetadataChanged(FabricCore::DFGExec exec, FTL::StrRef portPath, FTL::StrRef key, FTL::StrRef metadata) = 0;
      virtual void onNodePortMetadataChanged(FabricCore::DFGExec parent, FTL::StrRef nodePortPath, FTL::StrRef key, FTL::StrRef metadata) = 0;
      virtual void onExecPortTypeChanged(FabricCore::DFGExec exec, FTL::StrRef portPath, FTL::StrRef portType) = 0;
      virtual void onNodePortTypeChanged(FabricCore::DFGExec parent, FTL::StrRef nodePortPath, FTL::StrRef portType) = 0;

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
