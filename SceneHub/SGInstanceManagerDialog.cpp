// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "SGInstanceManagerDialog.h"
#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QColorDialog>
#include <QtGui/QFileDialog>
#include <iostream>
#include "macros.h"
#include <Core/Build.h>
using namespace std;
using namespace FabricUI;
using namespace FabricUI::SceneHub;


SGInstanceManagerDialog::SGInstanceManagerDialog(QWidget* parent, FabricCore::Client *client, FabricCore::RTVal testObject) {
  std::cerr << "SGInstanceManagerDialog::SGInstanceManagerDialog" << std::endl;
  
  init(parent, client, testObject);

  m_addInstance = false;
  m_colorButton = new QPushButton("Color");
  m_addButton = new QPushButton("Set");
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget( m_colorButton );
  mainLayout->addWidget( m_addButton );
 
  connect(m_colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
  connect(m_addButton, SIGNAL(clicked()), this, SLOT(updatetInstanceProperties()));
}

void SGInstanceManagerDialog::setInstanceProperties() {
  m_addInstance = false;
}

void SGInstanceManagerDialog::addInstance(QPoint screenPos) {
  m_addButton->setText("Add");
  m_glWidgetScreenPos = screenPos;
  m_addInstance = true;
}

// ***********

void SGInstanceManagerDialog::showColorDialog() {
  SGObjectManagerDialog::showColorDialog();
}

FabricCore::RTVal SGInstanceManagerDialog::showFileDialog() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Files (*.abc *.fbx)"));
  FabricCore::RTVal rtFileName;
  FABRIC_TRY_RETURN("SGInstanceManagerDialog::showFileDialog", FabricCore::RTVal(),
    rtFileName = FabricCore::RTVal::ConstructString(*m_client, fileName.toStdString().c_str());
  );
  return rtFileName;
}

void SGInstanceManagerDialog::updatetInstanceProperties() {
 
  if(m_addInstance)
  {
    FabricCore::RTVal rtFileName = showFileDialog();
  }
  else
  {
    std::cerr << "I am here" << std::endl;
    FABRIC_TRY("GLWidget::setInstanceProperties",
      m_testObject.callMethod("", "onSetInstanceProperties", 1, &m_color); 
      close();
    );
  }

  //  FABRIC_TRY("SGInstanceManagerDialog::setInstanceProperties",
  //    // Get the click position
  //    FabricCore::RTVal klViewportDim = m_viewport.callMethod("Vec2", "getDimensions", 0, 0);
  //    FabricCore::RTVal klPos = FabricCore::RTVal::Construct(*m_client, "Vec2", 0, 0);
  //    klPos.setMember("x", FabricCore::RTVal::ConstructFloat32(*m_client, float(m_glWidgetScreenPos.x())));
  //    // We must inverse the y coordinate to match Qt/RTR viewport system of coordonates
  //    klPos.setMember("y", FabricCore::RTVal::ConstructFloat32(*m_client, klViewportDim.maybeGetMember("y").getFloat32() - float(m_glWidgetScreenPos.y())));
  //    
  //    std::vector<FabricCore::RTVal> klParams(3); 
  //    klParams[0] = FabricCore::RTVal::ConstructUInt32(*m_client, m_InstanceType);
  //    klParams[1] = m_color;
  //    klParams[2] = klPos;
  //    m_testObject.callMethod("", "onAddInstance", 3, &klParams[0]); 
  //    close();
  //  );
  //}
}

