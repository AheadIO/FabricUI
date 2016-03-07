/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <QtCore/QString>
#include <QtGui/QVBoxLayout>
#include <FabricUI/SceneHub/TreeView/SHTreeViewWidget.h>

using namespace FabricCore;
using namespace FabricUI::SceneHub;


SHTreeViewWidget::SHTreeViewWidget(
  FabricCore::Client client,
  SHGLScene *shGLScene,
  FabricUI::DFG::DFGController *controller,
  QWidget *parent) 
  : QWidget(parent)
  , m_client(client)
  , m_controller(controller)
{ 
  init(m_client);
  setMainScene(shGLScene);
}

SHTreeViewWidget::SHTreeViewWidget(
  FabricCore::Client client,
  FabricUI::DFG::DFGController *controller,
  QWidget *parent) 
  : QWidget(parent) 
  , m_client(client)
  , m_controller(controller)
{
  init(m_client);
}

void SHTreeViewWidget::init(FabricCore::Client client) {
  m_bUpdatingSelectionFrom3D = false;
  m_shGLScene = 0;
  m_mainSHGLScene = 0;

  m_comboBox = new QComboBox();
  m_refreshButton = new QPushButton("Refresh");
  m_shTreeView = new FabricUI::SceneHub::SHTreeView(client);
  
  QLayout *layout = new QVBoxLayout();
  layout->addWidget(m_refreshButton);
  layout->addWidget(m_comboBox);
  layout->addWidget(m_shTreeView);
  this->setLayout(layout);

  QObject::connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(onUpdateSceneList()));
  QObject::connect(m_comboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onSetScene(const QString &)));
}

void SHTreeViewWidget::setScene(SHGLScene *shGLScene) {
 
  m_shGLScene = shGLScene;
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

void SHTreeViewWidget::setMainScene(SHGLScene *shGLScene) {
  m_mainSHGLScene = shGLScene;
  m_comboBox->addItem("Main Scene");
  setScene(shGLScene);
}

void SHTreeViewWidget::onSetScene(const QString &sceneName) {
  if(sceneName == "Main Scene") setScene(m_mainSHGLScene);
  else
  {
    FabricCore::DFGBinding binding = m_controller->getBinding();
    FabricCore::DFGExec exec = binding.getExec();
    if(exec.hasVar(sceneName.toUtf8().constData()))
    {
      m_shGLScene = new SHGLScene(
        &m_client,
        exec.getVarValue(sceneName.toUtf8().constData()));
      setScene(m_shGLScene);
    }
  }
}

void SHTreeViewWidget::onUpdateSceneList() {
  FabricCore::DFGBinding binding = m_controller->getBinding();
  FabricCore::DFGExec exec = binding.getExec();
 
  FabricCore::DFGStringResult json =  binding.getVars();
  FTL::JSONStrWithLoc jsonStrWithLoc( json.getCString() );
  FTL::OwnedPtr<FTL::JSONObject> jsonObject(FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>() );

  std::vector<FTL::JSONObject const *> objects;
  objects.push_back(jsonObject.get());
  QStringList sceneNameList;

  for(size_t i=0;i<objects.size();i++)
  {
    FTL::JSONObject const * varsObject = objects[i]->maybeGetObject( FTL_STR("vars") );
    if(varsObject)
    {
      for(FTL::JSONObject::const_iterator it = varsObject->begin(); it != varsObject->end(); it++)
      {
        FTL::CStrRef key = it->first;
        std::string name = key.c_str();

        FTL::JSONObject const *value = it->second->cast<FTL::JSONObject>();
        for(FTL::JSONObject::const_iterator jt = value->begin(); jt != value->end(); jt++) 
        {
          if(QString(jt->second->getStringValue().c_str()) == "SHGLScene")
          {
            if(!sceneNameList.contains(name.c_str()))
            sceneNameList.append(key.c_str());
          }
        }
      }
    }
  }

  m_comboBox->clear();
  if(m_mainSHGLScene) m_comboBox->addItem("Main Scene");
  for(size_t i=0; i<sceneNameList.size(); ++i)
    m_comboBox->addItem(sceneNameList[i]);
}


// **********************
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
        SHGLRenderer shGLRenderer(&m_shGLScene->getClient());// = new SHGLRenderer()
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
