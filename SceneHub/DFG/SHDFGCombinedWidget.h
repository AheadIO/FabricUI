// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef _SHDFGCOMBINEDWIDGET_H_
#define _SHDFGCOMBINEDWIDGET_H_
 
#include <FabricUI/Util/macros.h>
#include <FabricUI/DFG/DFGCombinedWidget.h>
#include <FabricUI/ValueEditor/VEEditorOwner.h>
#include <FabricUI/SceneHub/TreeView/SHTreeView.h>

using namespace FabricServices;
 
namespace FabricUI
{
  namespace DFG
  {
    class SHDFGCombinedWidget : public FabricUI::DFG::DFGCombinedWidget
    {
      Q_OBJECT
      
      public:
        SHDFGCombinedWidget(QWidget * parent);

        ~SHDFGCombinedWidget() {};
 
      public slots:
        virtual void onUndo() {};
        virtual void onRedo() {};

        virtual void onHotkeyPressed(Qt::Key key, Qt::KeyboardModifier modifier, QString str) { DFGCombinedWidget::onHotkeyPressed(key, modifier, str); }
        virtual void onGraphSet(FabricUI::GraphView::Graph * graph) { DFGCombinedWidget::onGraphSet(graph); };
        virtual void onNodeInspectRequested(FabricUI::GraphView::Node * node) { DFGCombinedWidget::onNodeInspectRequested(node); };
        virtual void onNodeEditRequested(FabricUI::GraphView::Node * node) { DFGCombinedWidget::onNodeEditRequested(node); };
        virtual void onAdditionalMenuActionsRequested(QString name, QMenu * menu, bool prefix) { DFGCombinedWidget::onAdditionalMenuActionsRequested(name, menu, prefix); }; 
        /// Displays the treeView in the application.
        /// \param initalExpandLevel The initial level of expension of the treeView.
        void showTreeView(unsigned int initalExpandLevel);
        /// Displays the treeView in the application.
        /// Expands the whole treeView.  
        void showTreeView();
        /// Updates the application when an item of the treeView is selected.
        /// \param item The selected item.
        void treeItemSelected(FabricUI::SceneHub::SHTreeItem *item);
        /// Updates the application when an item of the treeView is deselected.
        /// \param item The deselected item.
        void treeItemDeselected(FabricUI::SceneHub::SHTreeItem *item);
        /// Calls when the SceneGraph hierachy changed.
        void onSceneHierarchyChanged();  

      signals :
        void sceneHierarchyChanged();

      private slots:
        void onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog) { DFGCombinedWidget::onPortEditDialogCreated(dialog); }
        void onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog, FTL::JSONObjectEnc<> * additionalMetaData) { DFGCombinedWidget::onPortEditDialogInvoked(dialog, additionalMetaData); }
      
      private :
        /// Initializes the treeView widget.
        virtual void initTreeView();
        /// Initializes the windows docks.
        virtual void initDocks();
        void addSceneHubAsPort();
        
        SceneHub::SHTreeView *m_shTreeView;
        FabricCore::RTVal m_shHost;
    };

  }

}

#endif 
