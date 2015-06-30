// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGNotificationRouter__
#define __UI_DFG_DFGNotificationRouter__

#include <FabricCore.h>
#include <FTL/CStrRef.h>
#include <FTL/JSONValue.h>
#include <FabricUI/DFG/DFGConfig.h>

namespace FabricUI
{

  namespace DFG
  {
    class DFGController;
    class DFGWidget;

    class DFGNotificationRouter
    {
      friend class DFGController;
      friend class DFGWidget;

    public:

      DFGNotificationRouter(
        FabricCore::DFGBinding &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec,
        const DFGConfig & config = DFGConfig()
        );
      virtual ~DFGNotificationRouter() {}

      FabricCore::DFGBinding &getBinding()
        { return m_binding; }
      FTL::CStrRef getExecPath()
        { return m_execPath; }
      FabricCore::DFGExec &getExec()
        { return m_exec; }

      DFGController * getController()
        { return m_controller; }

      void setController(DFGController * controller)
        { m_controller = controller; }

    protected:

      virtual void onGraphSet();
      virtual void onNotification(FTL::CStrRef json);
      virtual void onNodeInserted(
        FTL::CStrRef nodeName,
        FTL::JSONObject const *jsonObject
        );
      virtual void onNodeRemoved(
        FTL::CStrRef nodeName
        );
      virtual void onNodePortInserted(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::JSONObject const *jsonObject
        );
      virtual void onNodePortRemoved(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName
        );
      virtual void onExecPortInserted(
        FTL::CStrRef portName,
        FTL::JSONObject const *jsonObject
        );
      virtual void onExecPortRemoved(
        FTL::CStrRef portName
        );
      virtual void onPortsConnected(
        FTL::CStrRef srcPath,
        FTL::CStrRef dstPath
        );
      virtual void onPortsDisconnected(
        FTL::CStrRef srcPath,
        FTL::CStrRef dstPath
        );
      virtual void onNodeMetadataChanged(
        FTL::CStrRef nodePath,
        FTL::CStrRef key,
        FTL::CStrRef value
        );
      virtual void onNodeTitleChanged(
        FTL::CStrRef nodePath,
        FTL::CStrRef title
        );
      virtual void onExecPortRenamed(
        FTL::CStrRef oldPortName,
        FTL::CStrRef newPortName,
        FTL::JSONObject const *execPortJSONObject
        );
      virtual void onNodePortRenamed(
        FTL::CStrRef nodeName,
        FTL::CStrRef oldPortName,
        FTL::CStrRef newPortName
        );
      virtual void onExecMetadataChanged(
        FTL::CStrRef key,
        FTL::CStrRef value
        );
      virtual void onExtDepAdded(
        FTL::CStrRef extension,
        FTL::CStrRef version
        );
      virtual void onExtDepRemoved(
        FTL::CStrRef extension,
        FTL::CStrRef version
        );
      virtual void onNodeCacheRuleChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef newCacheRule
        );
      virtual void onExecCacheRuleChanged(
        FTL::CStrRef newCacheRule
        );
      virtual void onExecPortResolvedTypeChanged(
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedType
        );
      virtual void onExecPortTypeSpecChanged(
        FTL::CStrRef portName,
        FTL::CStrRef typeSpec
        );
      virtual void onNodePortResolvedTypeChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedType
        );
      virtual void onExecPortMetadataChanged(
        FTL::CStrRef portName,
        FTL::CStrRef key,
        FTL::CStrRef value
        );
      virtual void onNodePortMetadataChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef key,
        FTL::CStrRef value
        );
      virtual void onExecPortTypeChanged(
        FTL::CStrRef portName,
        FTL::CStrRef execPortType
        );
      virtual void onNodePortTypeChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef nodePortType
        );
      virtual void onRefVarPathChanged(
        FTL::CStrRef refName,
        FTL::CStrRef newVarPath
        );
      virtual void onFuncCodeChanged(
        FTL::CStrRef code
        );

    private:

      void callback( FTL::CStrRef jsonStr );

      static void Callback(
        void *thisVoidPtr,
        char const *jsonCStr,
        uint32_t jsonSize
        )
      {
        static_cast<DFGNotificationRouter *>( thisVoidPtr )->callback(
          FTL::CStrRef( jsonCStr, jsonSize )
          );
      }

      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      FabricCore::DFGView m_view;
      DFGController * m_controller;
      DFGConfig m_config;
      bool m_performChecks;
    };

  };

};

#endif // __UI_DFG_DFGNotificationRouter__
