// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGWidget__
#define __UI_DFG_DFGWidget__

#include <QtCore/QSettings>
#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <Commands/CommandStack.h>
#include <FabricUI/GraphView/GraphHeaderWidget.h>
#include <FabricUI/DFG/Dialogs/DFGBaseDialog.h>
#include "DFGGraphViewWidget.h"
#include "DFGController.h"
#include "DFGNotificationRouter.h"
#include "DFGConfig.h"
#include "DFGKLEditorWidget.h"
#include "DFGTabSearchWidget.h"

#include <FTL/OwnedPtr.h>

namespace FabricUI
{
  namespace DFG
  {

    class DFGWidget : public QWidget
    {
      Q_OBJECT

    public:

      DFGWidget(
        QWidget * parent, 
        FabricCore::Client const &client,
        FabricCore::DFGHost const &dfgHost,
        FabricCore::DFGBinding const &binding,
        FabricCore::DFGExec const &graph,
        FabricServices::ASTWrapper::KLASTManager * manager,
        FabricServices::Commands::CommandStack * stack,
        const DFGConfig & dfgConfig = DFGConfig(),
        bool overTakeBindingNotifications = true
        );
      virtual ~DFGWidget();

      void setGraph(
        FabricCore::DFGHost const &coreDFGHost,
        FabricCore::DFGBinding const &coreDFGBinding,
        FabricCore::DFGExec const &coreDFGGraph,
        bool clear = true
        );

      GraphView::Graph * getUIGraph();
      DFGController * getUIController();
      DFGTabSearchWidget * getTabSearchWidget();
      DFGGraphViewWidget * getGraphViewWidget();

      bool isEditable() const { return m_isEditable; }
      static QSettings * getSettings();
      static void setSettings(QSettings * settings);

    signals:

      void newPresetSaved(QString presetFilePath);
      void onGraphSet(FabricUI::GraphView::Graph* graph);
      void portEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog);
      void portEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog);

    public slots:

      void onGoUpPressed();
      void onGraphAction(QAction * action);
      void onNodeAction(QAction * action);
      void onNodeEditRequested(FabricUI::GraphView::Node *);
      void onExecPortAction(QAction * action);
      void onSidePanelAction(QAction * action);
      void onHotkeyPressed(Qt::Key key, Qt::KeyboardModifier mod, QString name);
      void onHotkeyReleased(Qt::Key key, Qt::KeyboardModifier mod, QString name);
      void onNodeToBeRenamed(FabricUI::GraphView::Node* node);
      void onKeyPressed(QKeyEvent * event);
      void onBubbleEditRequested(FabricUI::GraphView::Node * node);

    private:

      static QMenu* graphContextMenuCallback(FabricUI::GraphView::Graph* graph, void* userData);
      static QMenu* nodeContextMenuCallback(FabricUI::GraphView::Node* node, void* userData);
      static QMenu* portContextMenuCallback(FabricUI::GraphView::Port* port, void* userData);
      static QMenu* sidePanelContextMenuCallback(FabricUI::GraphView::SidePanel* panel, void* userData);

      bool editNode(FabricCore::DFGExec exec, char const * name, bool pushExec = false);

      QPoint m_contextPos;
      FabricUI::GraphView::Node * m_contextNode;
      FabricUI::GraphView::Port * m_contextPort;
      FabricUI::GraphView::PortType m_contextPortType;

      DFGGraphViewWidget * m_uiGraphViewWidget;
      GraphView::GraphHeaderWidget * m_uiHeader;
      GraphView::Graph * m_uiGraph;
      FTL::OwnedPtr<DFGController> m_uiController;
      DFGNotificationRouter * m_router;
      DFGKLEditorWidget * m_klEditor;
      DFGTabSearchWidget * m_tabSearchWidget;
      FabricCore::Client m_coreClient;
      FabricCore::DFGHost m_coreDFGHost;
      FabricCore::DFGBinding m_coreDFGBinding;
      FabricCore::DFGExec m_coreDFGExec;
      FabricServices::ASTWrapper::KLASTManager * m_manager;
      DFGConfig m_dfgConfig;

      std::vector<FabricCore::DFGExec> m_coreDFGExecStack;
      std::string m_coreDFGExecPath;
      bool m_isEditable;

      static QSettings * g_settings;
    };

  };

};

#endif // __UI_DFG_DFGWidget__
