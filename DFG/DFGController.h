// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGController__
#define __UI_DFG_DFGController__

#include <DFGWrapper/Host.h>
#include <DFGWrapper/FuncExecutable.h>
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

      DFGController(GraphView::Graph * graph, FabricServices::Commands::CommandStack * stack, FabricCore::Client * client, FabricServices::DFGWrapper::Host * host, FabricServices::ASTWrapper::KLASTManager * manager, bool overTakeBindingNotifications = true);

      FabricServices::DFGWrapper::Host * getHost();
      FabricServices::DFGWrapper::Binding getBinding();
      FabricServices::DFGWrapper::GraphExecutablePtr getGraphExec();
      void setHost(FabricServices::DFGWrapper::Host * host);
      FabricCore::Client * getClient();
      void setClient(FabricCore::Client * client);
      DFGView * getView();
      void setView(DFGView * view);
      bool isViewingRootGraph();
      FabricServices::ASTWrapper::KLASTManager * astManager();

      virtual QString addNodeFromPreset(QString path, QString preset, QPointF pos);
      virtual QString addEmptyGraph(QString path, QString title, QPointF pos);
      virtual QString addEmptyFunc(QString path, QString title, QPointF pos);
      virtual bool removeNode(QString path);
      virtual bool removeNode(GraphView::Node * node);
      virtual bool renameNode(QString path, QString title);
      virtual bool renameNode(GraphView::Node * node, QString title);
      virtual GraphView::Pin * addPin(GraphView::Node * node, QString name, GraphView::PortType pType, QColor color, QString dataType = "");
      virtual bool removePin(GraphView::Pin * pin);
      virtual QString addPort(QString path, QString name, FabricCore::DFGPortType pType, QString dataType = "", bool setArgValue = true);
      virtual QString addPort(QString path, QString name, GraphView::PortType pType, QString dataType = "", bool setArgValue = true);
      virtual bool removePort(QString path, QString name);
      virtual GraphView::Port * addPortFromPin(GraphView::Pin * pin, GraphView::PortType pType);
      virtual QString renamePort(QString path, QString title);
      virtual bool addConnection(QString srcPath, QString dstPath, bool srcIsPin = true, bool dstIsPin = true);
      virtual bool addConnection(GraphView::ConnectionTarget * src, GraphView::ConnectionTarget * dst);
      virtual bool removeConnection(QString srcPath, QString dstPath, bool srcIsPin = true, bool dstIsPin = true);
      virtual bool removeConnection(GraphView::ConnectionTarget * src, GraphView::ConnectionTarget * dst);
      virtual bool removeAllConnections(QString path, bool isPin = true);
      virtual bool addExtensionDependency(QString extension, QString execPath, QString & errorMessage);
      virtual bool setCode(QString path, QString code);
      virtual QString reloadCode(QString path);
      virtual bool setArg(QString argName, QString dataType, QString json = "");
      virtual bool setArg(QString argName, FabricCore::RTVal value);
      virtual bool setDefaultValue(QString path, FabricCore::RTVal value);
      virtual bool setDefaultValue(QString path, QString dataType, QString json);
      virtual QString exportJSON(QString path);
      virtual bool setNodeCacheRule(QString path, FEC_DFGCacheRule rule);

      virtual bool moveNode(QString path, QPointF pos, bool isTopLeftPos = false);
      virtual bool moveNode(GraphView::Node * node, QPointF pos, bool isTopLeftPos = false);
      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool relaxNodes(QStringList paths = QStringList());

      virtual QString copy(QStringList paths = QStringList());
      virtual bool paste();
      virtual QString implodeNodes(QString desiredName, QStringList paths = QStringList());
      virtual QStringList explodeNode(QString path);

      virtual bool reloadExtensionDependencies(QString path);

      virtual void log(const char * message);
      virtual void logError(const char * message);

      virtual void setLogFunc(LogFunc func);

      virtual bool execute();
      bool bindUnboundRTVals(std::string dataType = "");

      virtual bool canConnectTo(QString pathA, QString pathB, QString &failureReason);

      virtual void populateNodeToolbar(GraphView::NodeToolbar * toolbar, GraphView::Node * node);

      FabricServices::DFGWrapper::NodePtr getNodeFromPath(const std::string & path);
      FabricServices::DFGWrapper::InstPtr getInstFromPath(const std::string & path);
      FabricServices::DFGWrapper::ExecutablePtr getExecFromPath(const std::string & path);
      FabricServices::DFGWrapper::ExecutablePtr getExecFromGlobalPath(const std::string & path);
      FabricServices::DFGWrapper::GraphExecutablePtr getGraphExecFromPath(const std::string & path);
      FabricServices::DFGWrapper::FuncExecutablePtr getFuncExecFromPath(const std::string & path);
      FabricServices::DFGWrapper::PortPtr getPortFromPath(const std::string & path);

      virtual QStringList getPresetPathsFromSearch(QString search, bool includePresets = true, bool includeNameSpaces = false);

    signals:

      void argsChanged();
      void structureChanged();
      void recompiled();
      void nodeEditRequested(FabricUI::GraphView::Node *);
      void portRenamed(QString path, QString newName);
      void argValueChanged(const char * argName);

    public slots:

      void onValueChanged(ValueItem * item);
      void checkErrors();
      void nodeToolTriggered(FabricUI::GraphView::Node *, QString);

    private:

      static void bindingNotificationCallback(void * userData, char const *jsonCString, uint32_t jsonLength);
      void updatePresetPathDB();

      FabricCore::Client * m_client;
      FabricServices::DFGWrapper::Host * m_host;
      FabricServices::ASTWrapper::KLASTManager * m_manager;
      DFGView * m_view;
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
