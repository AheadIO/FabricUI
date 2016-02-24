// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <iostream>

#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QMenuBar>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include "SHDFGCombinedWidget.h"
#include <FabricUI/Style/FabricStyle.h>
#include <FabricUI/DFG/Dialogs/DFGNodePropertiesDialog.h>
#include <FabricUI/DFG/DFGActions.h>

using namespace FabricUI::DFG;
 
 
void SHDFGCombinedWidget::initTreeView() {
  DFGCombinedWidget::initTreeView();
  m_shTreeView = new FabricUI::SceneHub::SHTreeView(m_client);
}

void SHDFGCombinedWidget::initDocks() { 
  DFGCombinedWidget::initDocks(); 
  m_LineEdit = new QLineEdit("");
  QWidget *widget = new QWidget();
  QVBoxLayout *layout = new QVBoxLayout();
  widget->setLayout(layout);
  layout->addWidget(m_LineEdit);
  layout->addWidget(m_shTreeView);
  m_hSplitter->addWidget(widget);
  
  QObject::connect(m_LineEdit, SIGNAL(editingFinished()), this, SLOT(refresh()));
}

void SHDFGCombinedWidget::refresh() {
  
  FabricCore::DFGBinding binding = m_dfgWidget->getUIController()->getBinding();
  FabricCore::DFGExec exec = binding.getExec();
  
  QString shHostName = m_LineEdit->text();
  if(exec.hasVar(shHostName.toUtf8())) 
  {
    FabricCore::RTVal shHost = exec.getVarValue(shHostName.toUtf8());
    if(shHost.isNullObject()) return;
    
    m_shHostName = shHostName;
    if(shHost.isValid()) std::cout << "I am valid" << std::endl;
    if(shHost.isObject()) std::cout << "I am an object" << std::endl;
    if(shHost.isNullObject()) std::cout << "I am a null object" << std::endl;
 
    //FABRIC_TRY( "SHDFGCombinedWidget::initTreeView",
      FabricCore::RTVal sceneGraphRTVal = shHost.callMethod("SceneGraph", "getScene", 0, 0);
      if(sceneGraphRTVal.isNullObject()) return;

      //NOTE: this doesn't work, returns crap: maybeGetMember("root");
      FabricCore::RTVal sceneRootSGObjectRTVal = sceneGraphRTVal.callMethod("SGObject", "getRoot", 0, 0);
      FabricCore::RTVal materialRootSGObjectRTVal = shHost.callMethod("SGObject", "getMaterialLibraryRoot", 0, 0);
      FabricCore::RTVal imagesRootSGObjectRTVal = shHost.callMethod("SGObject", "getImageLibraryRoot", 0, 0);
      FabricCore::RTVal assetsRootSGObjectRTVal = shHost.callMethod("SGObject", "getAssetLibraryRoot", 0, 0);

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
    
      showTreeView(1);
    //);
    std::cout << "I am a boss 3 " << std::endl;  
  }
}

// ***** Tree-View *****
void SHDFGCombinedWidget::showTreeView(unsigned int initalExpandLevel) {
  if(initalExpandLevel == uint32_t(-1)) m_shTreeView->expandAll();
  else if(initalExpandLevel > 0) m_shTreeView->expandToDepth(initalExpandLevel-1);
}

void SHDFGCombinedWidget::showTreeView() {
  showTreeView(1);
}

void SHDFGCombinedWidget::onSceneHierarchyChanged() {
  // Check if it actually changed, to reduce number of notifications
  //if(shHost.callMethod("Boolean", "sceneHierarchyChanged", 0, 0).getBoolean())
  //  emit sceneHierarchyChanged();
}

void SHDFGCombinedWidget::treeItemSelected(FabricUI::SceneHub::SHTreeItem *item) { 
  //FABRIC_TRY( "SHDFGCombinedWidget::treeItemSelected",
  //  FabricCore::RTVal sgObj = item->getSGObject();
  //  shHost.callMethod( "", "treeItemSelected", 1, &sgObj );     
  //);
}

void SHDFGCombinedWidget::treeItemDeselected(FabricUI::SceneHub::SHTreeItem *item) {
  //FABRIC_TRY( "SHDFGCombinedWidget::treeItemDeselected",
  //  FabricCore::RTVal sgObj = item->getSGObject();
  //  shHost.callMethod( "", "treeItemDeselected", 1, &sgObj );
  //);
}