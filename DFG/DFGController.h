//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGController__
#define __UI_DFG_DFGController__

#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/ValueEditor/ValueItem.h>
#include <SplitSearch/SplitSearch.hpp>
#include <vector>
#include <ASTWrapper/KLASTManager.h>

using namespace FabricUI::ValueEditor;

namespace FabricUI
{

  namespace DFG
  {

    class DFGUICmdHandler;
    class DFGNotificationRouter;

    class DFGController : public GraphView::Controller
    {

      friend class DFGValueEditor;
      friend class DFGWidget;

      Q_OBJECT

    public:

      typedef void(*LogFunc)(const char * message);

      DFGController(
        GraphView::Graph * graph,
        FabricCore::Client client,
        FabricServices::ASTWrapper::KLASTManager * manager,
        FabricCore::DFGHost host,
        FabricCore::DFGBinding binding,
        DFGUICmdHandler *cmdHandler,
        FabricServices::Commands::CommandStack * stack,
        bool overTakeBindingNotifications = true
        );
      ~DFGController();

      FabricCore::Client &getClient()
        { return m_client; }
      FabricCore::DFGHost &getHost()
        { return m_host; }
      FabricCore::DFGBinding &getBinding()
        { return m_binding; }
      FTL::CStrRef getExecPath();
      FabricCore::DFGExec &getExec();

      void setHost( FabricCore::DFGHost const &coreDFGHost );
      void setBinding( FabricCore::DFGBinding const &coreDFGBinding );
      void setClient( FabricCore::Client const &coreClient );
      DFGUICmdHandler *getCmdHandler() const
        { return m_cmdHandler; }
      DFGNotificationRouter * getRouter();
      void setRouter(DFGNotificationRouter * router);
      bool isViewingRootGraph();
      FabricServices::ASTWrapper::KLASTManager * astManager();

      // Parent virtual functions

      virtual bool gvcDoRemoveNodes(
        FTL::ArrayRef<GraphView::Node *> nodes
        );

      virtual bool gvcDoAddConnection(
        GraphView::ConnectionTarget * src,
        GraphView::ConnectionTarget * dst
        );

      virtual bool gvcDoRemoveConnection(
        GraphView::ConnectionTarget * src,
        GraphView::ConnectionTarget * dst
        );
      
      virtual void gvcDoAddPort(
        FTL::CStrRef desiredPortName,
        GraphView::PortType portType,
        FTL::CStrRef typeSpec = FTL::CStrRef(),
        GraphView::ConnectionTarget *connectWith = 0
        );

      // Commands

      void cmdRemoveNodes(
        FTL::ArrayRef<FTL::StrRef> nodeNames
        );

      void cmdConnect(
        FTL::StrRef srcPath, 
        FTL::StrRef dstPath
        );

      void cmdDisconnect(
        FTL::StrRef srcPath, 
        FTL::StrRef dstPath
        );

      std::string cmdAddInstWithEmptyGraph(
        FTL::CStrRef title,
        QPointF pos
        );

      std::string cmdAddInstWithEmptyFunc(
        FTL::CStrRef title,
        FTL::CStrRef initialCode,
        QPointF pos
        );

      std::string cmdAddInstFromPreset(
        FTL::CStrRef presetPath,
        QPointF pos
        );

      std::string cmdAddVar(
        FTL::CStrRef desiredNodeName,
        FTL::StrRef dataType,
        FTL::StrRef extDep,
        QPointF pos
        );

      std::string cmdAddGet(
        FTL::CStrRef desiredNodeName,
        FTL::StrRef varPath,
        QPointF pos
        );

      std::string cmdAddSet(
        FTL::CStrRef desiredNodeName,
        FTL::StrRef varPath,
        QPointF pos
        );

      std::string cmdAddPort(
        FTL::CStrRef desiredPortName,
        FabricCore::DFGPortType dfgPortType,
        FTL::CStrRef typeSpec,
        FTL::CStrRef portToConnect
        );

      void cmdRemovePort(
        FTL::CStrRef portName
        );

      std::string cmdImplodeNodes(
        FTL::CStrRef desiredNodeName,
        FTL::ArrayRef<FTL::CStrRef> nodeNames
        );

      std::vector<std::string> cmdExplodeNode(
        FTL::CStrRef nodeName
        );

      virtual bool renameNodeByPath(char const * path, char const * title);
      virtual bool renameNode(GraphView::Node * node, FTL::CStrRef title);
      virtual std::string renamePortByPath(char const *  path, char const *  title);
      virtual bool addExtensionDependency(char const *  extension, char const *  execPath, std::string  & errorMessage);
      virtual bool setCode(char const *  path, char const *  code);
      virtual std::string reloadCode(char const *  path);
      virtual bool setArg(char const *  argName, char const *  dataType, char const *  json = "");
      virtual bool setArg(char const *  argName, FabricCore::RTVal value);
      virtual bool setDefaultValue(char const *  path, FabricCore::RTVal value);
      virtual bool setDefaultValue(char const *  path, char const *  dataType, char const *  json);
      virtual std::string exportJSON(char const *  path);
      virtual bool setNodeCacheRule(char const *  path, FEC_DFGCacheRule rule);
      virtual bool setRefVarPath(char const *  path, char const * varPath);

      virtual bool moveNode(char const * path, QPointF pos, bool isTopLeftPos = false);
      std::string addBackDropNode(char const * title, QPointF pos);
      virtual bool moveBackDropNode(GraphView::BackDropNode * node, QPointF pos, bool isTopLeftPos = false);
      bool removeBackDropNode(GraphView::BackDropNode * node);
      bool renameBackDropNode(GraphView::BackDropNode * node, char const * title);
      virtual bool moveNode(GraphView::Node * node, QPointF pos, bool isTopLeftPos = false);
      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool relaxNodes(QStringList paths = QStringList());
      virtual bool tintBackDropNode(GraphView::BackDropNode * node, QColor color);
      virtual bool setNodeComment(GraphView::Node * node, char const * comment);
      virtual bool setNodeCommentExpanded(GraphView::Node * node, bool expanded);

      virtual std::string copy(QStringList paths = QStringList());
      virtual bool paste();

      virtual bool reloadExtensionDependencies(char const * path);

      virtual void log(const char * message);
      virtual void logError(const char * message);

      virtual void setLogFunc(LogFunc func);

      virtual bool execute();
      bool bindUnboundRTVals(FTL::StrRef dataType = FTL::StrRef());

      virtual bool canConnectTo(
        char const *pathA,
        char const *pathB,
        std::string &failureReason
        );

      virtual void populateNodeToolbar(GraphView::NodeToolbar * toolbar, GraphView::Node * node);
      virtual bool setBackDropNodeSize(GraphView::BackDropNode * node, QSizeF size);

      virtual QStringList getPresetPathsFromSearch(char const * search, bool includePresets = true, bool includeNameSpaces = false);

      virtual DFGNotificationRouter * createRouter(
        FabricCore::DFGBinding &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec
        );

      static QStringList getVariableWordsFromBinding(FabricCore::DFGBinding & binding, FTL::CStrRef currentExecPath);

      void emitArgsChanged()
        { emit argsChanged(); }
      void emitStructureChanged()
        { emit structureChanged(); }
      void emitRecompiled()
        { emit recompiled(); }

    signals:

      void argsChanged();
      void structureChanged();
      void recompiled();
      void nodeEditRequested(FabricUI::GraphView::Node *);
      void execPortRenamed(char const * path, char const * newName);
      void argValueChanged(const char * argName);
      void variablesChanged();

    public slots:

      void onValueChanged(ValueItem * item);
      void checkErrors();
      void nodeToolTriggered(FabricUI::GraphView::Node *, char const *);
      void onVariablesChanged();

    private:

      void bindingNotificationCallback( FTL::CStrRef jsonStr );
      static void BindingNotificationCallback(
        void * userData,
        char const *jsonCString,
        uint32_t jsonLength
        )
      {
        static_cast<DFGController *>( userData )->bindingNotificationCallback(
          FTL::CStrRef( jsonCString, jsonLength )
          );
      }

      void updatePresetPathDB();

      FabricCore::Client m_client;
      FabricCore::DFGHost m_host;
      FabricCore::DFGBinding m_binding;
      FabricServices::ASTWrapper::KLASTManager * m_manager;
      DFGUICmdHandler *m_cmdHandler;
      DFGNotificationRouter * m_router;
      LogFunc m_logFunc;
      bool m_overTakeBindingNotifications;
      FabricServices::SplitSearch::Dict m_presetNameSpaceDict;
      FabricServices::SplitSearch::Dict m_presetPathDict;
      std::vector<std::string> m_presetNameSpaceDictSTL;
      std::vector<std::string> m_presetPathDictSTL;
      bool m_presetDictsUpToDate;
    };

  };

};

#endif // __UI_DFG_DFGController__
