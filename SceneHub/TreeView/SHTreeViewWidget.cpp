/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <QtCore/QString>
#include <QtGui/QVBoxLayout>
#include <FabricUI/SceneHub/TreeView/SHTreeViewWidget.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace FabricUI::SceneHub;

SHTreeViewWidget::SHTreeViewWidget(
  FabricCore::Client client,
  DFG::DFGController *controller,
  QWidget *parent) 
  : QWidget(parent) 
  , m_client(client)
  , m_controller(controller)
{
  m_treeModel = 0;
  m_mainSHGLScene = 0;
  m_bUpdatingSelectionFrom3D = false;
  m_shGLScene = new SHGLScene(m_client);

  m_comboBox = new QComboBox();
  m_refreshButton = new QPushButton("Refresh");
  m_shTreeView = new FabricUI::SceneHub::SHTreeView(client);
  
  QLayout *layout = new QVBoxLayout();
  layout->addWidget(m_refreshButton);
  layout->addWidget(m_comboBox);
  layout->addWidget(m_shTreeView);
  this->setLayout(layout);

  QObject::connect( m_shTreeView, SIGNAL( itemSelected( FabricUI::SceneHub::SHTreeItem * ) ), this, SLOT( treeItemSelected( FabricUI::SceneHub::SHTreeItem * ) ) );
  QObject::connect( m_shTreeView, SIGNAL( itemDeselected( FabricUI::SceneHub::SHTreeItem * ) ), this, SLOT( treeItemDeselected( FabricUI::SceneHub::SHTreeItem * ) ) );
  QObject::connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(onUpdateSceneList()));
  QObject::connect(m_comboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onUpdateScene(const QString &)));
}

SHTreeViewWidget::SHTreeViewWidget(
  SHGLScene *shGLScene,
  DFG::DFGController *controller,
  QWidget *parent) 
  : SHTreeViewWidget(shGLScene->getClient(), controller, parent)
{ 
  m_mainSHGLScene = shGLScene;
  m_comboBox->addItem("Main Scene");
  constructTree();
}

void SHTreeViewWidget::resetTree() {
  m_shTreeView->reset();
  if(m_treeModel)
  {
    delete m_treeModel;
    m_treeModel = 0;
  }
}

void SHTreeViewWidget::constructTree() {
  resetTree();

  m_treeModel = new FabricUI::SceneHub::SHTreeModel(m_shGLScene->getClient(), m_shGLScene->getSG(), m_shTreeView);
  QObject::connect(this, SIGNAL(sceneHierarchyChanged()), m_treeModel, SLOT(onSceneHierarchyChanged()));
  QObject::connect(m_treeModel, SIGNAL(sceneHierarchyChanged()), this, SLOT(onSceneHierarchyChanged()));

  QModelIndex sceneRootIndex = m_treeModel->addRootItem(m_shGLScene->getSceneRoot());
  m_treeModel->addRootItem(m_shGLScene->getAssetLibraryRoot());
  m_treeModel->addRootItem(m_shGLScene->getMaterialLibraryRoot());
  m_treeModel->addRootItem(m_shGLScene->getImageLibraryRoot());

  m_shTreeView->setModel(m_treeModel);
  m_shTreeView->setExpanded(sceneRootIndex, true);
}

void SHTreeViewWidget::onUpdateScene(const QString &sceneName) {
  if(sceneName == "Main Scene") 
  {
    m_shGLScene->setSHGLScene(m_mainSHGLScene->getSHGLScene());
    constructTree();
    emit sceneUpdated(m_shGLScene);
  }
  else if(m_controller->getBinding().getExec().hasVar(sceneName.toUtf8().constData()))
  {
    m_shGLScene->setSHGLScene(m_controller->getBinding().getExec().getVarValue(sceneName.toUtf8().constData()));
    constructTree();
    emit sceneUpdated(m_shGLScene);
  }
  else 
  {
    m_comboBox->clear();
    resetTree();
  }
}

void SHTreeViewWidget::onUpdateSceneList() {
  m_comboBox->clear();

  FabricCore::DFGStringResult json =  m_controller->getBinding().getVars();
  FTL::JSONStrWithLoc jsonStrWithLoc( json.getCString() );
  FTL::OwnedPtr<FTL::JSONObject> jsonObject(FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>() );

  std::vector<FTL::JSONObject const *> objects;
  objects.push_back(jsonObject.get());
  QStringList sceneNameList;

  for(size_t i=0; i<objects.size(); i++)
  {
    FTL::JSONObject const * varsObject = objects[i]->maybeGetObject( FTL_STR("vars") );
    if(varsObject)
    {
      for(FTL::JSONObject::const_iterator it = varsObject->begin(); it != varsObject->end(); it++)
      {
        QString sceneName(it->first.c_str());
        FTL::JSONObject const *value = it->second->cast<FTL::JSONObject>();
        for(FTL::JSONObject::const_iterator jt = value->begin(); jt != value->end(); jt++) 
        {
          if(QString(jt->second->getStringValue().c_str()) == "SHGLScene")
          {
            if(!sceneNameList.contains(sceneName))
              sceneNameList.append(sceneName);
          }
        }
      }
    }
  }

  if(sceneNameList.size() == 0 && !m_mainSHGLScene)
    resetTree();

  if(m_mainSHGLScene) m_comboBox->addItem("Main Scene");
  for(size_t i=0; i<sceneNameList.size(); ++i)
    m_comboBox->addItem(sceneNameList[i]);
}


// **********************
void SHTreeViewWidget::expandTree(uint32_t level) {
  if(level == uint32_t(-1)) m_shTreeView->expandAll();
  else if(level > 0) m_shTreeView->expandToDepth(level - 1);
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
    if( !m_bUpdatingSelectionFrom3D )
    {
      m_shGLScene->treeItemSelected( item->getSGObject() );
      emit sceneHierarchyChanged();
    }
  }
}

void SHTreeViewWidget::treeItemDeselected(FabricUI::SceneHub::SHTreeItem *item) {
  if(m_shGLScene)
  {
    if(!m_bUpdatingSelectionFrom3D) 
    {
      m_shGLScene->treeItemDeselected(item->getSGObject()); 
      emit sceneHierarchyChanged();
    }
  }
}

void SHTreeViewWidget::updateFrom3DSelection() {
  if(m_shGLScene)
  {
    // Ensure there's not a loopback from TreeView selection change to 3D view
    m_bUpdatingSelectionFrom3D = true;
    // Ensure it really changed; we will be called for any accepted event; only a few will be selection changes
    if(m_shGLScene->selectionChangedFromManips())
    {
      m_shTreeView->setSelectedObjects(m_shGLScene->getSelectedObjects());
      emit sceneHierarchyChanged();
    }
    m_bUpdatingSelectionFrom3D = false;
  }
}
