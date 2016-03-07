/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_TREE_VIEW_WIDGET_H__
#define __UI_SCENEHUB_TREE_VIEW_WIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/SceneHub/SHGLScene.h>
#include <FabricUI/SceneHub/SHGLRenderer.h>
#include <FabricUI/SceneHub/TreeView/SHTreeView.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHTreeViewWidget : public QWidget { 

      Q_OBJECT

      public:
        SHTreeViewWidget(
          FabricCore::Client client,
          SHGLScene *shGLScene, 
          FabricUI::DFG::DFGController *controller,
          QWidget *parent = 0);

        SHTreeViewWidget(
          FabricCore::Client client,
          FabricUI::DFG::DFGController *controller,
          QWidget *parent = 0);

        ~SHTreeViewWidget() {}

        void init(FabricCore::Client client);

        void setScene(SHGLScene *shGLScene);

        void setMainScene(SHGLScene *shGLScene);

        

      public slots:
        void onSetScene(const QString &sceneName);

        void onUpdateSceneList();

        void expandTree(uint32_t level);
        /// Calls when the SceneGraph hierachy changed.
        /// Calls when the SceneGraph hierachy changed.
        void onSceneHierarchyChanged();
        /// Updates the application when an item of the treeView is selected.
        /// \param item The selected item.
        void treeItemSelected(FabricUI::SceneHub::SHTreeItem *item);
        /// Updates the application when an item of the treeView is deselected.
        /// \param item The deselected item.
        void treeItemDeselected(FabricUI::SceneHub::SHTreeItem *item);
        /// Updates the treeView from the 3D scene selection.
        void updateFrom3DSelection();


      signals:
        void sceneHierarchyChanged();
 

      protected:
        SHGLScene *m_shGLScene;
        SHGLScene *m_mainSHGLScene;
        FabricUI::DFG::DFGController *m_controller;
        SHTreeView *m_shTreeView;
        QPushButton *m_refreshButton;
        QComboBox *m_comboBox;
        bool m_bUpdatingSelectionFrom3D;
        FabricCore::Client m_client;

    };
  }
}

#endif __UI_SCENEHUB_TREE_VIEW_WIDGET_H__