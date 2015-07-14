// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGWidget__
#define __UI_DFG_DFGWidget__

#include <QtCore/QSettings>
#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <Commands/CommandStack.h>
#include <FabricUI/DFG/DFGExecHeaderWidget.h>
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
    class DFGExecHeaderWidget;

    class DFGWidget : public QWidget
    {
      Q_OBJECT

    public:

      DFGWidget(
        QWidget * parent, 
        FabricCore::Client &client,
        FabricCore::DFGHost &host,
        FabricCore::DFGBinding &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec,
        FabricServices::ASTWrapper::KLASTManager * manager,
        FabricServices::Commands::CommandStack * stack,
        const DFGConfig & dfgConfig = DFGConfig(),
        bool overTakeBindingNotifications = true
        );
      virtual ~DFGWidget();

      DFGController *getDFGController()
        { return m_uiController.get(); }

      GraphView::Graph * getUIGraph();
      DFGController * getUIController();
      DFGTabSearchWidget * getTabSearchWidget();
      DFGGraphViewWidget * getGraphViewWidget();

      void setExecExtDeps( FTL::CStrRef extDeps );

      bool isEditable() const { return m_isEditable; }
      static QSettings * getSettings();
      static void setSettings(QSettings * settings);
      
      void refreshExtDeps( FTL::CStrRef extDeps );

    signals:

      void execChanged();
      void newPresetSaved(QString presetFilePath);
      void onGraphSet(FabricUI::GraphView::Graph* graph);
      void portEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog);
      void portEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog);

    public slots:

      void onExecChanged();
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

      bool editNode(
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec
        );

      QPoint m_contextPos;
      FabricUI::GraphView::Node * m_contextNode;
      FabricUI::GraphView::Port * m_contextPort;
      FabricUI::GraphView::PortType m_contextPortType;

      DFGGraphViewWidget * m_uiGraphViewWidget;
      DFGExecHeaderWidget * m_uiHeader;
      GraphView::Graph * m_uiGraph;
      FTL::OwnedPtr<DFGController> m_uiController;
      DFGNotificationRouter * m_router;
      DFGKLEditorWidget * m_klEditor;
      DFGTabSearchWidget * m_tabSearchWidget;
      FabricServices::ASTWrapper::KLASTManager * m_manager;
      DFGConfig m_dfgConfig;

      bool m_isEditable;

      static QSettings * g_settings;
    };

  };

};

#endif // __UI_DFG_DFGWidget__
