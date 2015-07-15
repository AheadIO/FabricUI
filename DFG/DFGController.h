// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

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

    class DFGNotificationRouter;
    class DFGWidget;

    class DFGController : public GraphView::Controller
    {
      Q_OBJECT

      friend class DFGValueEditor;
      friend class DFGWidget;

    public:

      typedef void(*LogFunc)(const char * message);

      DFGController(
        GraphView::Graph * graph,
        DFGWidget *dfgWidget,
        FabricCore::Client &client,
        FabricServices::ASTWrapper::KLASTManager * manager,
        FabricServices::Commands::CommandStack * stack,
        bool overTakeBindingNotifications = true
        );
      ~DFGController();

      DFGWidget *getDFGWidget() const
        { return m_dfgWidget; }
      
      FabricCore::Client &getClient()
        { return m_coreClient; }
      FabricCore::DFGHost &getCoreDFGHost()
        { return m_coreDFGHost; }
      FabricCore::DFGBinding &getCoreDFGBinding()
        { return m_coreDFGBinding; }
      FTL::CStrRef getCoreDFGExecPath()
        { return m_coreDFGExecPath; }
      FabricCore::DFGExec &getCoreDFGExec()
        { return m_coreDFGExec; }

      void setClient( FabricCore::Client const &coreClient );
      void setHostBindingExec(
        FabricCore::DFGHost &host,
        FabricCore::DFGBinding &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec
        );
      void setBindingExec(
        FabricCore::DFGBinding &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec
        );
      void setExec(
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec
        );

      DFGNotificationRouter * getRouter();
      void setRouter(DFGNotificationRouter * router);

      bool isViewingRootGraph()
        { return m_coreDFGExecPath.empty(); }
      FabricServices::ASTWrapper::KLASTManager * astManager()
        { return m_manager; }

      virtual std::string addDFGNodeFromPreset(FTL::StrRef preset, QPointF pos);
      virtual std::string addDFGVar(FTL::StrRef varName, FTL::StrRef dataType, FTL::StrRef extDep, QPointF pos);
      virtual std::string addDFGGet(FTL::StrRef varName, FTL::StrRef varPath, QPointF pos);
      virtual std::string addDFGSet(FTL::StrRef varName, FTL::StrRef varPath, QPointF pos);
      virtual std::string addEmptyGraph(char const * title, QPointF pos);
      virtual std::string addEmptyFunc(char const * title, QPointF pos);
      virtual void addBackDropNode( FTL::CStrRef title, QPointF pos );
      virtual bool removeNode(char const * path);
      virtual bool removeNode(GraphView::Node * node);
      virtual bool renameNodeByPath(char const *path, char const *title);
      virtual bool renameNode(GraphView::Node * node, FTL::StrRef title);
      virtual GraphView::Pin * addPin(GraphView::Node * node, FTL::StrRef name, GraphView::PortType pType, QColor color, FTL::StrRef dataType = "");
      virtual bool removePin(GraphView::Pin * pin);
      virtual std::string addPortByPath(
        FTL::StrRef execPath,
        FTL::StrRef name,
        FabricCore::DFGPortType pType,
        FTL::StrRef dataType = FTL::StrRef(),
        bool setArgValue = true
        );
      virtual std::string addPortByPath(
        FTL::StrRef execPath,
        FTL::StrRef name,
        GraphView::PortType pType,
        FTL::StrRef dataType = FTL::StrRef(),
        bool setArgValue = true
        );
      virtual bool removePortByName(char const *name);
      virtual GraphView::Port * addPortFromPin(GraphView::Pin * pin, GraphView::PortType pType);
      virtual std::string renamePortByPath(char const *path, char const *title);
      virtual bool addConnection(char const *  srcPath, char const *  dstPath);
      virtual bool addConnection(GraphView::ConnectionTarget * src, GraphView::ConnectionTarget * dst);
      virtual bool removeConnectionByPath(char const *srcPath, char const *dstPath);
      virtual bool removeConnection(GraphView::ConnectionTarget * src, GraphView::ConnectionTarget * dst);
      virtual bool removeAllConnections(char const *  path);
      virtual bool addExtensionDependency(char const *  extension, char const *  execPath, std::string  & errorMessage);
      virtual bool setCode( FTL::CStrRef code );
      virtual std::string reloadCode();
      virtual bool setArg(char const *  argName, char const *  dataType, char const *  json = "");
      virtual bool setArg(char const *  argName, FabricCore::RTVal value);
      virtual bool setDefaultValue(char const *  path, FabricCore::RTVal value);
      virtual bool setDefaultValue(char const *  path, char const *  dataType, char const *  json);
      virtual std::string exportJSON(char const *  path);
      virtual bool setNodeCacheRule(char const *  path, FEC_DFGCacheRule rule);
      virtual bool setRefVarPath(char const *  path, char const * varPath);

      virtual bool moveNode(char const * path, QPointF pos, bool isTopLeftPos = false);
      virtual bool moveNode(GraphView::Node * node, QPointF pos, bool isTopLeftPos = false);
      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool relaxNodes(QStringList paths = QStringList());
      virtual bool tintBackDropNode(GraphView::BackDropNode * node, QColor color);
      virtual bool setNodeComment(GraphView::Node * node, char const * comment);
      virtual void setNodeCommentExpanded(GraphView::Node * node, bool expanded);

      virtual std::string copy(QStringList paths = QStringList());
      virtual bool paste();
      virtual std::string implodeNodes(char const * desiredName, QStringList paths = QStringList());
      virtual QStringList explodeNode(char const * path);

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

      virtual DFGNotificationRouter * createRouter();

      static QStringList getVariableWordsFromBinding(FabricCore::DFGBinding & binding, FTL::CStrRef currentExecPath);

    signals:

      void hostChanged();
      void bindingChanged();
      void execChanged();
      void argsChanged();
      void structureChanged();
      void recompiled();
      void nodeEditRequested(FabricUI::GraphView::Node *);
      void nodeDeleted(QString nodePath);
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

      DFGWidget *m_dfgWidget;
      FabricCore::Client m_coreClient;
      FabricCore::DFGHost m_coreDFGHost;
      FabricCore::DFGBinding m_coreDFGBinding;
      std::string m_coreDFGExecPath;
      FabricCore::DFGExec m_coreDFGExec;
      FabricServices::ASTWrapper::KLASTManager * m_manager;
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
