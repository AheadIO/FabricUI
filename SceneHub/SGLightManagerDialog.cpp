// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "SGLightManagerDialog.h"
#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QColorDialog>
#include <iostream>
#include "macros.h"
#include <Core/Build.h>
using namespace std;
using namespace FabricUI;
using namespace FabricUI::SceneHub;


SGLightManagerDialog::SGLightManagerDialog(QWidget* parent, FabricCore::Client *client, FabricCore::RTVal testObject) {
  std::cerr << "SGLightManagerDialog::SGLightManagerDialog" << std::endl;
  init(parent, client, testObject);

  m_addLight = false;
  m_colorButton = new QPushButton("Color");
  m_addButton = new QPushButton("Set");
  m_intensitySlider = new QSlider( Qt::Horizontal);
  m_intensitySlider->setMinimum(0);
  m_intensitySlider->setMaximum(10);
  m_intensitySlider->setValue(5);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget( m_intensitySlider );
  mainLayout->addWidget( m_colorButton );
  mainLayout->addWidget( m_addButton );
 
  connect(m_colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
  connect(m_intensitySlider, SIGNAL(valueChanged(int)), this, SLOT(updateLightProperties()));
  connect(m_addButton, SIGNAL(clicked()), this, SLOT(setLightProperties()));
}

void SGLightManagerDialog::setLightProperties(uint32_t lightType) {
  m_addLight = false;
  m_lightType = lightType;
}

void SGLightManagerDialog::addLight(uint32_t lightType, QPoint screenPos) {
  m_addButton->setText("Add");
  m_glWidgetScreenPos = screenPos;
  m_lightType = lightType;
  m_addLight = true;
}

// ***********

void SGLightManagerDialog::showColorDialog() {
  SGObjectManagerDialog::showColorDialog();
  updateLightProperties();
}

void SGLightManagerDialog::updateLightProperties() {
  FabricCore::RTVal intensity = FabricCore::RTVal::ConstructFloat32(*m_client, float(m_intensitySlider->value()/10.0f));
  // Here we should be able to dynamically update the scene light
  // if(!m_addLight)
  // m_parent->update
}

void SGLightManagerDialog::setLightProperties() {
  if(m_addLight)
  {
    FABRIC_TRY("SGLightManagerDialog::setLightProperties",
      // Get the click position
      FabricCore::RTVal klViewportDim = m_viewport.callMethod("Vec2", "getDimensions", 0, 0);
      FabricCore::RTVal klPos = FabricCore::RTVal::Construct(*m_client, "Vec2", 0, 0);
      klPos.setMember("x", FabricCore::RTVal::ConstructFloat32(*m_client, float(m_glWidgetScreenPos.x())));
      // We must inverse the y coordinate to match Qt/RTR viewport system of coordonates
      klPos.setMember("y", FabricCore::RTVal::ConstructFloat32(*m_client, klViewportDim.maybeGetMember("y").getFloat32() - float(m_glWidgetScreenPos.y())));
      
      std::vector<FabricCore::RTVal> klParams(3); 
      klParams[0] = FabricCore::RTVal::ConstructUInt32(*m_client, m_lightType);
      klParams[1] = m_color;
      klParams[2] = klPos;
      m_testObject.callMethod("", "onAddLight", 3, &klParams[0]); 
      close();
    );
  }
}

