/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <QtGui/QVBoxLayout>
#include <FabricUI/SceneHub/TreeView/SHTreeViewWidget.h>

using namespace FabricCore;
using namespace FabricUI::SceneHub;


SHTreeViewWidget::SHTreeViewWidget(
  SHGLScene *shGLScene,
  //SHGLRenderer *shGLRenderer,
  QWidget *parent) 
  : QWidget(parent)
  //, m_shGLScene(shGLScene)
  //, m_shGLRenderer(shGLRenderer)
{ 
  init(shGLScene);
}

SHTreeViewWidget::SHTreeViewWidget(
  //SHGLScene *shGLScene,
  //SHGLRenderer *shGLRenderer,
  QWidget *parent) 
  : QWidget(parent) 
{
  m_shTreeView = 0;
  m_shGLScene = 0;
  m_comboBox = new QComboBox();//"graph");
  QLayout *layout = new QVBoxLayout();
  layout->addWidget(m_comboBox);
  this->setLayout(layout);
}

void SHTreeViewWidget::init(SHGLScene *shGLScene) {
  m_shGLScene = shGLScene;
  m_bUpdatingSelectionFrom3D = false;
  m_comboBox = new QComboBox();//"graph");
  m_shTreeView = new FabricUI::SceneHub::SHTreeView(m_shGLScene->getClient());

  //setScene(shGLScene);
  QLayout *layout = new QVBoxLayout();
  layout->addWidget(m_comboBox);
  layout->addWidget(m_shTreeView);
  this->setLayout(layout);

 
  SHTreeModel *treeModel = new FabricUI::SceneHub::SHTreeModel(m_shGLScene->getClient(), m_shGLScene->getSG(), m_shTreeView);
  QObject::connect(this, SIGNAL(sceneHierarchyChanged()), treeModel, SLOT(onSceneHierarchyChanged()));
  QObject::connect(treeModel, SIGNAL(sceneHierarchyChanged()), this, SLOT(onSceneHierarchyChanged()));

  QModelIndex sceneRootIndex = treeModel->addRootItem(m_shGLScene->getSceneRoot());
  treeModel->addRootItem(m_shGLScene->getAssetLibraryRoot());
  treeModel->addRootItem(m_shGLScene->getMaterialLibraryRoot());
  treeModel->addRootItem(m_shGLScene->getImageLibraryRoot());

  m_shTreeView->setModel(treeModel);
  m_shTreeView->setExpanded(sceneRootIndex, true);
}

void SHTreeViewWidget::expandTree(uint32_t level) {
  if(m_shTreeView)
  {
    if(level == uint32_t(-1)) m_shTreeView->expandAll();
    else if(level > 0) m_shTreeView->expandToDepth(level - 1);
  }
}

void SHTreeViewWidget::onSceneHierarchyChanged() {
  // Check if it actually changed, to reduce number of notifications
  if(m_shGLScene)
  {
    if(m_shGLScene->sceneHierarchyChanged())
      emit sceneHierarchyChanged();
  }
}

void SHTreeViewWidget::treeItemSelected(FabricUI::SceneHub::SHTreeItem *item) {
  if(m_shGLScene)
  {
    if(m_shTreeView)
    {
      if(!m_bUpdatingSelectionFrom3D) 
      {
        m_shGLScene->treeItemSelected(item->getSGObject()); 
        emit sceneHierarchyChanged();
      }
    }
  }
}

void SHTreeViewWidget::treeItemDeselected(FabricUI::SceneHub::SHTreeItem *item) {
  if(m_shGLScene)
  {
    if(m_shTreeView)
    {
      if(!m_bUpdatingSelectionFrom3D) 
      {
        m_shGLScene->treeItemDeselected(item->getSGObject()); 
        emit sceneHierarchyChanged();
      }
    }
  }
}

void SHTreeViewWidget::updateFrom3DSelection() {
  if(m_shGLScene)
  {
    if(m_shTreeView)
    {
      // Ensure there's not a loopback from TreeView selection change to 3D view
      m_bUpdatingSelectionFrom3D = true;
       /// Gets a reference to the scenegraph.
      FabricCore::RTVal scene;
      if(m_shGLScene->getSHGLScene(scene))
      {
        SHGLRenderer shGLRenderer( m_shGLScene->getClient() );// = new SHGLRenderer()

        // Ensure it really changed; we will be called for any accepted event; only a few will be selection changes
        if(shGLRenderer.selectionChangedFromManips())
        {
          m_shTreeView->setSelectedObjects(m_shGLScene->getSelectedObjects());//selectedObjects);
          emit sceneHierarchyChanged();
        }
      }

      m_bUpdatingSelectionFrom3D = false;
    }
  }
}
