// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>

#include "SHDFGCombinedWidget.h"
#include <FabricUI/Style/FabricStyle.h>
#include <FabricUI/DFG/Dialogs/DFGNodePropertiesDialog.h>
#include <FabricUI/DFG/DFGActions.h>

using namespace FabricUI::DFG;
 
SHDFGCombinedWidget::SHDFGCombinedWidget(QWidget * parent)
:DFGCombinedWidget(parent)
{
};
 
void SHDFGCombinedWidget::initTreeView() {
  
  DFGCombinedWidget::initTreeView();
  m_shTreeView = new FabricUI::SceneHub::SHTreeView(m_client);

  /*
  FabricCore::RTVal sceneGraphRTVal;
  FabricCore::RTVal sceneRootSGObjectRTVal;
  FabricCore::RTVal materialRootSGObjectRTVal;
  FabricCore::RTVal imagesRootSGObjectRTVal;
  FabricCore::RTVal assetsRootSGObjectRTVal;

  FABRIC_TRY( "SceneHubWindow::initTreeView",
    sceneGraphRTVal = m_shObject.callMethod("SceneGraph", "getScene", 0, 0);
    if( sceneGraphRTVal.isNullObject() ) return;

    //NOTE: this doesn't work, returns crap: maybeGetMember("root");
    sceneRootSGObjectRTVal = sceneGraphRTVal.callMethod("SGObject", "getRoot", 0, 0);
    materialRootSGObjectRTVal = m_shObject.callMethod("SGObject", "getMaterialLibraryRoot", 0, 0);
    imagesRootSGObjectRTVal = m_shObject.callMethod("SGObject", "getImageLibraryRoot", 0, 0);
    assetsRootSGObjectRTVal = m_shObject.callMethod("SGObject", "getAssetLibraryRoot", 0, 0);

    FabricUI::SceneHub::SHTreeModel *treeModel = new FabricUI::SceneHub::SHTreeModel(m_client, sceneGraphRTVal, m_shTreeView);
    QModelIndex sceneRootIndex = treeModel->addRootItem(sceneRootSGObjectRTVal);
    treeModel->addRootItem(materialRootSGObjectRTVal);
    treeModel->addRootItem(imagesRootSGObjectRTVal);
    treeModel->addRootItem(assetsRootSGObjectRTVal);

    m_shTreeView->setModel(treeModel);
    m_shTreeView->setExpanded( sceneRootIndex, true);

    QObject::connect(m_shTreeView, SIGNAL( itemSelected( FabricUI::SceneHub::SHTreeItem * ) ), this, SLOT( treeItemSelected( FabricUI::SceneHub::SHTreeItem * ) ));
    QObject::connect(m_shTreeView, SIGNAL( itemDeselected( FabricUI::SceneHub::SHTreeItem * ) ), this, SLOT( treeItemDeselected( FabricUI::SceneHub::SHTreeItem * ) ));
    QObject::connect(this, SIGNAL( sceneHierarchyChanged() ), treeModel, SLOT( onSceneHierarchyChanged() ));
    QObject::connect(treeModel, SIGNAL( sceneHierarchyChanged() ), this, SLOT( onSceneHierarchyChanged() ));
  );
  */
}

void SHDFGCombinedWidget::initDocks() { 
  m_hSplitter->addWidget(m_shTreeView);
  DFGCombinedWidget::initDocks(); 
};
