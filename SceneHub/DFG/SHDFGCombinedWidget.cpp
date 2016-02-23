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
  m_shHost = FabricCore::RTVal::Create(m_client, "SceneHubGLHost", 0, 0);

  DFGCombinedWidget::initTreeView();
  m_shTreeView = new FabricUI::SceneHub::SHTreeView(m_client);

  FabricCore::RTVal sceneGraphRTVal;
  FabricCore::RTVal sceneRootSGObjectRTVal;
  FabricCore::RTVal materialRootSGObjectRTVal;
  FabricCore::RTVal imagesRootSGObjectRTVal;
  FabricCore::RTVal assetsRootSGObjectRTVal;

  FABRIC_TRY( "SHDFGCombinedWidget::initTreeView",
    sceneGraphRTVal = m_shHost.callMethod("SceneGraph", "getScene", 0, 0);
    if( sceneGraphRTVal.isNullObject() ) return;

    //NOTE: this doesn't work, returns crap: maybeGetMember("root");
    sceneRootSGObjectRTVal = sceneGraphRTVal.callMethod("SGObject", "getRoot", 0, 0);
    materialRootSGObjectRTVal = m_shHost.callMethod("SGObject", "getMaterialLibraryRoot", 0, 0);
    imagesRootSGObjectRTVal = m_shHost.callMethod("SGObject", "getImageLibraryRoot", 0, 0);
    assetsRootSGObjectRTVal = m_shHost.callMethod("SGObject", "getAssetLibraryRoot", 0, 0);

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
}

void SHDFGCombinedWidget::initDocks() { 
  DFGCombinedWidget::initDocks(); 
  m_hSplitter->addWidget(m_shTreeView);

  addSceneHubAsPort();
};

void SHDFGCombinedWidget::addSceneHubAsPort() {
  FabricCore::DFGBinding binding = m_dfgWidget->getUIController()->getBinding();
  FabricCore::DFGExec exec = binding.getExec();
  try
  {
    //std::string inPort = exec.addExecPort("SHGLHost", FabricCore::DFGPortType_In, "SceneHubGLHost");
    exec.addVar("mySHGLHost", "SceneHubGLHost", "SceneHub");
    binding.execute();
    exec.setVarValue("mySHGLHost", m_shHost);
    //binding.setArgValue("SHGLHost", m_shHost);
    binding.execute();
  }
  catch(FabricCore::Exception e)
  {
    printf("Caught FabricCore::Exception: %s\n", e.getDesc_cstr());
  }
}

// ***** Tree-View *****
void SHDFGCombinedWidget::showTreeView(unsigned int initalExpandLevel) {
  if( initalExpandLevel == uint32_t( -1 ) ) m_shTreeView->expandAll();
  else if( initalExpandLevel > 0 ) m_shTreeView->expandToDepth( initalExpandLevel - 1);
}

void SHDFGCombinedWidget::showTreeView() {
  showTreeView(1);
}

void SHDFGCombinedWidget::onSceneHierarchyChanged() {
  // Check if it actually changed, to reduce number of notifications
  if( m_shHost.callMethod( "Boolean", "sceneHierarchyChanged", 0, 0 ).getBoolean() )
    emit sceneHierarchyChanged();
}

void SHDFGCombinedWidget::treeItemSelected(FabricUI::SceneHub::SHTreeItem *item) { 
  //FABRIC_TRY( "SHDFGCombinedWidget::treeItemSelected",
  //  FabricCore::RTVal sgObj = item->getSGObject();
  //  m_shHost.callMethod( "", "treeItemSelected", 1, &sgObj );     
  //);
}

void SHDFGCombinedWidget::treeItemDeselected(FabricUI::SceneHub::SHTreeItem *item) {
  //FABRIC_TRY( "SHDFGCombinedWidget::treeItemDeselected",
  //  FabricCore::RTVal sgObj = item->getSGObject();
  //  m_shHost.callMethod( "", "treeItemDeselected", 1, &sgObj );
  //);
}