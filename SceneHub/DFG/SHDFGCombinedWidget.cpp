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
  m_shTreeViewWidget = new SceneHub::SHTreeViewWidget(this);
  QObject::connect(m_shTreeViewWidget, SIGNAL(updateScene()), this, SIGNAL(setScene()));
}

void SHDFGCombinedWidget::initDocks() { 
  DFGCombinedWidget::initDocks(); 
  m_hSplitter->addWidget(m_shTreeViewWidget);
}

void SHDFGCombinedWidget::refresh() {

}

void SHDFGCombinedWidget::setScene() {
  FabricCore::DFGBinding binding = m_dfgWidget->getUIController()->getBinding();
  FabricCore::DFGExec exec = binding.getExec();
  
  QString sceneName = "TRUX";//m_LineEdit->text();
  if(exec.hasVar(sceneName.toUtf8())) 
  {
    if(m_shGLScene) delete m_shGLScene;
    m_shGLScene = new SceneHub::SHGLScene(m_client, exec.getVarValue(sceneName.toUtf8()));
    m_shTreeViewWidget->init(m_shGLScene);
  }
}
