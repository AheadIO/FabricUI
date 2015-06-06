// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGController__
#define __UI_DFG_DFGController__

#include <GraphView/Controller.h>
#include <GraphView/Node.h>
#include <GraphView/Pin.h>
#include <GraphView/Port.h>
#include <ValueEditor/ValueItem.h>
#include "DFGView.h"
#include <SplitSearch/SplitSearch.hpp>
#include <vector>
#include <ASTWrapper/KLASTManager.h>

using namespace FabricUI::ValueEditor;

namespace FabricUI
{

  namespace DFG
  {

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
        FabricCore::DFGExec exec,
        FabricServices::Commands::CommandStack * stack,
        bool overTakeBindingNotifications = true
        );

      FabricCore::Client const &getClient()
        { return m_coreClient; }
      FabricCore::DFGHost const &getCoreDFGHost()
        { return m_coreDFGHost; }
      FabricCore::DFGBinding const &getCoreDFGBinding();
      FabricCore::DFGExec const &getCoreDFGExec();

      void setHost( FabricCore::DFGHost const &coreDFGHost );
      void setClient( FabricCore::Client const &coreClient );
      DFGView * getView();
      void setView(DFGView * view);
      bool isViewingRootGraph();
      FabricServices::ASTWrapper::KLASTManager * astManager();

      virtual std::string addNodeFromPreset(char const * path, char const * preset, QPointF pos);
      virtual std::string addEmptyGraph(char const * path, char const * title, QPointF pos);
      virtual std::string addEmptyFunc(char const * path, char const * title, QPointF pos);
      virtual bool removeNode(char const * path);
      virtual bool removeNode(GraphView::Node * node);
      virtual bool renameNode(char const * path, char const * title);
      virtual bool renameNode(GraphView::Node * node, char const * title);
      virtual GraphView::Pin * addPin(GraphView::Node * node, char const * name, GraphView::PortType pType, QColor color, char const * dataType = "");
      virtual bool removePin(GraphView::Pin * pin);
      virtual std::string addPort(char const *  path, char const *  name, FabricCore::DFGPortType pType, char const *  dataType = "", bool setArgValue = true);
      virtual std::string addPort(char const *  path, char const *  name, GraphView::PortType pType, char const *  dataType = "", bool setArgValue = true);
      virtual bool removePort(char const *  name);
      virtual GraphView::Port * addPortFromPin(GraphView::Pin * pin, GraphView::PortType pType);
      virtual std::string renamePort(char const *  path, char const *  title);
      virtual bool addConnection(char const *  srcPath, char const *  dstPath);
      virtual bool addConnection(GraphView::ConnectionTarget * src, GraphView::ConnectionTarget * dst);
      virtual bool removeConnection(char const *srcPath, char const *dstPath);
      virtual bool removeConnection(GraphView::ConnectionTarget * src, GraphView::ConnectionTarget * dst);
      virtual bool removeAllConnections(char const *  path);
      virtual bool addExtensionDependency(char const *  extension, char const *  execPath, std::string  & errorMessage);
      virtual bool setCode(char const *  path, char const *  code);
      virtual std::string reloadCode(char const *  path);
      virtual bool setArg(char const *  argName, char const *  dataType, char const *  json = "");
      virtual bool setArg(char const *  argName, FabricCore::RTVal value);
      virtual bool setDefaultValue(char const *  path, FabricCore::RTVal value);
      virtual bool setDefaultValue(char const *  path, char const *  dataType, char const *  json);
      virtual std::string exportJSON(char const *  path);
      virtual bool setNodeCacheRule(char const *  path, FEC_DFGCacheRule rule);

      virtual bool moveNode(char const * path, QPointF pos, bool isTopLeftPos = false);
      virtual bool moveNode(GraphView::Node * node, QPointF pos, bool isTopLeftPos = false);
      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool relaxNodes(QStringList paths = QStringList());

      virtual std::string copy(QStringList paths = QStringList());
      virtual bool paste();
      virtual std::string implodeNodes(char const * desiredName, QStringList paths = QStringList());
      virtual QStringList explodeNode(char const * path);

      virtual bool reloadExtensionDependencies(char const * path);

      virtual void log(const char * message);
      virtual void logError(const char * message);

      virtual void setLogFunc(LogFunc func);

      virtual char const * getExecPath()
        {  return m_execPath.c_str(); }

      virtual bool execute();
      bool bindUnboundRTVals(std::string dataType = "");

      virtual bool canConnectTo(
        char const *pathA,
        char const *pathB,
        std::string &failureReason
        );

      virtual void populateNodeToolbar(GraphView::NodeToolbar * toolbar, GraphView::Node * node);

      virtual QStringList getPresetPathsFromSearch(char const * search, bool includePresets = true, bool includeNameSpaces = false);

    signals:

      void argsChanged();
      void structureChanged();
      void recompiled();
      void nodeEditRequested(FabricUI::GraphView::Node *);
      void execPortRenamed(char const * path, char const * newName);
      void argValueChanged(const char * argName);

    public slots:

      void onValueChanged(ValueItem * item);
      void checkErrors();
      void nodeToolTriggered(FabricUI::GraphView::Node *, char const *);

    private:

      static void bindingNotificationCallback(void * userData, char const *jsonCString, uint32_t jsonLength);
      void updatePresetPathDB();

      FabricCore::Client m_coreClient;
      FabricCore::DFGHost m_coreDFGHost;
      FabricCore::DFGBinding m_coreDFGBinding;
      FabricServices::ASTWrapper::KLASTManager * m_manager;
      DFGView * m_view;
      LogFunc m_logFunc;
      bool m_overTakeBindingNotifications;
      FabricServices::SplitSearch::Dict m_presetNameSpaceDict;
      FabricServices::SplitSearch::Dict m_presetPathDict;
      std::vector<std::string> m_presetNameSpaceDictSTL;
      std::vector<std::string> m_presetPathDictSTL;
      bool m_presetDictsUpToDate;
      std::string m_execPath;
    };

  };

};

#endif // __UI_DFG_DFGController__
