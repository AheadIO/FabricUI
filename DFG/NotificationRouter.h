// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_NotificationRouter__
#define __UI_DFG_NotificationRouter__

#include <FTL/JSONValue.h>
#include <FTL/StrRef.h>
#include <FabricUI/GraphView/Controller.h>
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

      virtual void onNotification(FTL::CStrRef json) = 0;
      virtual void onNodeInserted(
        FTL::CStrRef nodeName,
        FTL::JSONObject const *jsonObject
        ) = 0;
      virtual void onNodeRemoved(
        FTL::CStrRef nodeName
        ) = 0;
      virtual void onNodePortInserted(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::JSONObject const *jsonObject
        ) = 0;
      virtual void onNodePortRemoved(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName
        ) = 0;
      virtual void onExecPortInserted(
        FTL::CStrRef portName,
        FTL::JSONObject const *jsonObject
        ) = 0;
      virtual void onExecPortRemoved(
        FTL::CStrRef portName
        ) = 0;
      virtual void onPortsConnected(
        FTL::CStrRef srcPath,
        FTL::CStrRef dstPath
        ) = 0;
      virtual void onPortsDisconnected(
        FTL::CStrRef srcPath,
        FTL::CStrRef dstPath
        ) = 0;
      virtual void onNodeMetadataChanged(
        FTL::CStrRef nodePath,
        FTL::CStrRef key,
        FTL::CStrRef metadata
        ) = 0;
      virtual void onNodeTitleChanged(
        FTL::CStrRef nodePath,
        FTL::CStrRef title
        ) = 0;
      virtual void onExecPortRenamed(
        FTL::CStrRef oldPortName,
        FTL::CStrRef newPortName,
        FTL::JSONObject const *execPortJSONObject
        ) = 0;
      virtual void onNodePortRenamed(
        FTL::CStrRef nodeName,
        FTL::CStrRef oldPortName,
        FTL::CStrRef newPortName
        ) = 0;
      virtual void onExecMetadataChanged(
        FTL::CStrRef key,
        FTL::CStrRef value
        ) = 0;
      virtual void onExtDepAdded(
        FTL::CStrRef extension,
        FTL::CStrRef version
        ) = 0;
      virtual void onExtDepRemoved(
        FTL::CStrRef extension,
        FTL::CStrRef version
        ) = 0;
      virtual void onNodeCacheRuleChanged(
        FTL::CStrRef path,
        FTL::CStrRef rule
        ) = 0;
      virtual void onExecCacheRuleChanged(
        FTL::CStrRef rule
        ) = 0;
      virtual void onExecPortResolvedTypeChanged(
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedType
        ) = 0;
      virtual void onExecPortTypeSpecChanged(
        FTL::CStrRef portName,
        FTL::CStrRef typeSpec
        ) = 0;
      virtual void onNodePortResolvedTypeChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedType
        ) = 0;
      virtual void onExecPortMetadataChanged(
        FTL::CStrRef portName,
        FTL::CStrRef key,
        FTL::CStrRef value
        ) = 0;
      virtual void onNodePortMetadataChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef key,
        FTL::CStrRef value
        ) = 0;
      virtual void onExecPortTypeChanged(
        FTL::CStrRef portName,
        FTL::CStrRef execPortType
        ) = 0;
      virtual void onNodePortTypeChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef nodePortType
        ) = 0;

    private:

      void callback( FTL::CStrRef jsonStr );

      static void Callback(
        void *thisVoidPtr,
        char const *jsonCStr,
        uint32_t jsonSize
        )
      {
        static_cast<NotificationRouter *>( thisVoidPtr )->callback(
          FTL::CStrRef( jsonCStr, jsonSize )
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
