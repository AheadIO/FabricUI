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
        DFGUICmdHandler *cmdHandler,
        bool overTakeBindingNotifications = true
        );
      ~DFGController();

      DFGWidget *getDFGWidget() const
        { return m_dfgWidget; }
      
      FabricCore::Client &getClient()
        { return m_client; }
      FabricCore::DFGHost &getHost()
        { return m_host; }
      FabricCore::DFGBinding &getBinding()
        { return m_binding; }
      FTL::CStrRef getExecPath()
        { return m_execPath; }
      FabricCore::DFGExec &getExec()
        { return m_exec; }

      DFGUICmdHandler *getCmdHandler() const
        { return m_cmdHandler; }

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
      void refreshExec();

      DFGNotificationRouter * getRouter();
      void setRouter(DFGNotificationRouter * router);

      bool isViewingRootGraph() const
        { return m_execPath.empty(); }
      FabricServices::ASTWrapper::KLASTManager * astManager()
        { return m_manager; }

      // returns true if the user allows a preset edit
      bool validPresetSplit() const;

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
      
      virtual bool gvcDoAddInstFromPreset(
        FTL::CStrRef presetPath,
        QPointF pos
        );

      virtual void gvcDoAddPort(
        FTL::CStrRef desiredPortName,
        GraphView::PortType portType,
        FTL::CStrRef typeSpec = FTL::CStrRef(),
        GraphView::ConnectionTarget *connectWith = 0,
        FTL::StrRef extDep = FTL::StrRef(),
        FTL::CStrRef metaData = FTL::CStrRef()
        );

      virtual void gvcDoSetNodeCommentExpanded(
        GraphView::Node *node,
        bool expanded
        );

      virtual void gvcDoMoveNodes(
        std::vector<GraphView::Node *> const &nodes,
        QPointF delta,
        bool allowUndo
        );

      virtual void gvcDoResizeBackDropNode(
        GraphView::BackDropNode *backDropNode,
        QPointF newTopLeftPos,
        QSizeF newSize,
        bool allowUndo
        );

      // Commands

      void cmdRemoveNodes(
        FTL::ArrayRef<FTL::StrRef> nodeNames
        );

      void cmdConnect(
        FTL::CStrRef srcPath, 
        FTL::CStrRef dstPath
        );

      void cmdDisconnect(
        FTL::CStrRef srcPath, 
        FTL::CStrRef dstPath
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
        FTL::CStrRef dataType,
        FTL::CStrRef extDep,
        QPointF pos
        );

      std::string cmdAddGet(
        FTL::CStrRef desiredNodeName,
        FTL::CStrRef varPath,
        QPointF pos
        );

      std::string cmdAddSet(
        FTL::CStrRef desiredNodeName,
        FTL::CStrRef varPath,
        QPointF pos
        );

      std::string cmdAddPort(
        FTL::CStrRef desiredPortName,
        FabricCore::DFGPortType dfgPortType,
        FTL::CStrRef typeSpec,
        FTL::CStrRef portToConnect,
        FTL::StrRef extDep,
        FTL::CStrRef uiMetadata
        );

      std::string cmdEditPort(
        FTL::StrRef oldPortName,
        FTL::StrRef desiredNewPortName,
        FTL::StrRef typeSpec,
        FTL::StrRef extDep,
        FTL::StrRef uiMetadata
        );

      void cmdRemovePort(
        FTL::CStrRef portName
        );

      void cmdMoveNodes(
        FTL::ArrayRef<FTL::StrRef> nodeNames,
        FTL::ArrayRef<QPointF> newTopLeftPoss
        );

      void cmdResizeBackDropNode(
        FTL::CStrRef backDropNodeName,
        QPointF newTopLeftPos,
        QSizeF newSize
        );

      std::string cmdImplodeNodes(
        FTL::ArrayRef<FTL::StrRef> nodeNames,
        FTL::CStrRef desiredNodeName
        );

      std::vector<std::string> cmdExplodeNode(
        FTL::CStrRef nodeName
        );

      void cmdAddBackDrop(
        FTL::CStrRef title,
        QPointF pos
        );

      void cmdSetTitle(
        FTL::CStrRef newTitle
        );

      void cmdSetNodeComment(
        FTL::CStrRef nodeName, 
        FTL::CStrRef comment
        );

      void setNodeCommentExpanded(
        FTL::CStrRef nodeName, 
        bool expanded
        );

      void setNodeToolTip(
        FTL::CStrRef nodeName, 
        FTL::CStrRef newToolTip
        );

      void setNodeDocUrl(
        FTL::CStrRef nodeName, 
        FTL::CStrRef newDocUrl
        );

      void cmdSetCode( FTL::CStrRef code );

      std::string cmdRenameNode(
        FTL::CStrRef oldName,
        FTL::CStrRef desiredNewName
        );

      std::string cmdRenameExecPort(
        FTL::CStrRef oldName,
        FTL::CStrRef desiredNewName
        );

      void cmdCut();

      void cmdPaste();

      void cmdSetArgType(
        FTL::CStrRef argName,
        FTL::CStrRef typeName
        );

      void cmdSetArgValue(
        FTL::CStrRef argName,
        FabricCore::RTVal const &value
        );

      bool cmdSetDefaultValue(
        FabricCore::DFGBinding &binding,
        FTL::CStrRef execPath,
        FabricCore::DFGExec &exec,
        FTL::CStrRef portPath,
        FabricCore::RTVal const &value
        );

      void cmdSetRefVarPath(
        FabricCore::DFGBinding &binding,
        FTL::CStrRef execPath,
        FabricCore::DFGExec &exec,
        FTL::CStrRef refName,
        FTL::CStrRef varPath
        );

      void cmdReorderPorts(
        FabricCore::DFGBinding &binding,
        FTL::CStrRef execPath,
        FabricCore::DFGExec &exec,
        const std::vector<unsigned int> & indices
        );

      void cmdSetExtDeps(
        FTL::ArrayRef<FTL::StrRef> nameAndVers
        );

      void cmdSplitFromPreset();

      virtual std::string reloadCode();

      virtual bool zoomCanvas(float zoom);
      virtual bool panCanvas(QPointF pan);
      virtual bool relaxNodes(QStringList paths = QStringList());
      virtual bool setNodeColor(const char * nodeName, const char * key, QColor color);
      /// Sets the collpase state of the selected node.
      /// Saves it in the node preferences    
      virtual void setSelectedNodeCollapseState(int collpaseState);
      virtual bool removeNodeColor(const char * nodeName, const char * key);
      virtual bool setNodeBackgroundColor(const char * nodeName, QColor color);
      virtual bool setNodeHeaderColor(const char * nodeName, QColor color);
      virtual bool removeNodeHeaderColor(const char * nodeName);
      virtual bool setNodeTextColor(const char * nodeName, QColor color);
      
      virtual std::string copy();

      virtual bool reloadExtensionDependencies(char const * path);

      virtual void log(const char * message);
      virtual void logError(const char * message);

      virtual void setLogFunc(LogFunc func);

      void execute();

      bool bindUnboundRTVals();
      static bool bindUnboundRTVals(FabricCore::Client &client, FabricCore::DFGBinding &binding);

      virtual bool canConnectTo(
        char const *pathA,
        char const *pathB,
        std::string &failureReason
        );

      virtual QStringList getPresetPathsFromSearch(char const * search, bool includePresets = true, bool includeNameSpaces = false);

      virtual DFGNotificationRouter *createRouter();

      static QStringList getVariableWordsFromBinding(FabricCore::DFGBinding & binding, FTL::CStrRef currentExecPath);

      void emitVarsChanged()
      {
        if ( m_updateSignalBlockCount > 0 )
          m_varsChangedPending = true;
        else
          emit varsChanged();
      }

      void emitArgsChanged()
      {
        if ( m_updateSignalBlockCount > 0 )
          m_argsChangedPending = true;
        else
          emit argsChanged();
      }

      void emitArgValuesChanged()
      {
        if ( m_updateSignalBlockCount > 0 )
          m_argValuesChangedPending = true;
        else
          emit argValuesChanged();
      }

      void emitDefaultValuesChanged()
      {
        if ( m_updateSignalBlockCount > 0 )
          m_defaultValuesChangedPending = true;
        else
          emit defaultValuesChanged();
      }

      void emitDirty()
      {
        if ( m_updateSignalBlockCount > 0 )
          m_dirtyPending = true;
        else
          emit dirty();
      }

      void emitExecSplitChanged()
      {
        emit execSplitChanged();
      }

      void setBlockCompilations( bool blockCompilations );

      class UpdateSignalBlocker
      {
      public:

        UpdateSignalBlocker( DFGController *controller )
          : m_controller( controller )
        {
          ++m_controller->m_updateSignalBlockCount;
        }

        ~UpdateSignalBlocker()
        {
          if ( --m_controller->m_updateSignalBlockCount == 0 )
          {
            if ( m_controller->m_varsChangedPending )
            {
              m_controller->m_varsChangedPending = false;
              emit m_controller->varsChanged();
            }
            if ( m_controller->m_argsChangedPending )
            {
              m_controller->m_argsChangedPending = false;
              emit m_controller->argsChanged();
            }
            if ( m_controller->m_argValuesChangedPending )
            {
              m_controller->m_argValuesChangedPending = false;
              emit m_controller->argValuesChanged();
            }
            if ( m_controller->m_defaultValuesChangedPending )
            {
              m_controller->m_defaultValuesChangedPending = false;
              emit m_controller->defaultValuesChanged();
            }
            if ( m_controller->m_dirtyPending )
            {
              m_controller->m_dirtyPending = false;
              emit m_controller->dirty();
            }
          }
        }

      private:

        DFGController *m_controller;
      };

      void emitNodeRemoved( FTL::CStrRef nodeName );

    signals:

      void hostChanged();
      void bindingChanged( FabricCore::DFGBinding const &newBinding );
      void execChanged();

      void varsChanged();
      void argsChanged();
      void argValuesChanged();
      void defaultValuesChanged();
      void dirty();
      void execSplitChanged();

      void nodeEditRequested(FabricUI::GraphView::Node *);
      void execPortRenamed(char const * path, char const * newName);

      void nodeRemoved( FTL::CStrRef nodePathFromRoot );

    public slots:

      void onValueItemDelta( ValueItem *valueItem );
      void onValueItemInteractionEnter( ValueItem *valueItem );
      void onValueItemInteractionDelta( ValueItem *valueItem );
      void onValueItemInteractionLeave( ValueItem *valueItem );

      void checkErrors();
      void onVariablesChanged();
      virtual void onNodeHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button);

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
      FabricCore::Client m_client;
      FabricCore::DFGHost m_host;
      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
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

      uint32_t m_updateSignalBlockCount;
      bool m_varsChangedPending;
      bool m_argsChangedPending;
      bool m_argValuesChangedPending;
      bool m_defaultValuesChangedPending;
      bool m_dirtyPending;
    };

  };

};

#endif // __UI_DFG_DFGController__
