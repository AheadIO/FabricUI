// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/SceneHub/Managers/SGLightManagerDialog.h>
#include <FabricUI/Util/macros.h>
#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QColorDialog>
#include <iostream>
#include <Core/Build.h>
using namespace std;
using namespace FabricUI;
using namespace FabricUI::SceneHub;


SGLightManagerDialog::SGLightManagerDialog(QWidget* parent, FabricCore::Client *client, FabricCore::RTVal testObject) {

  init(parent, client, testObject);

  m_colorButton = new QPushButton("Color");
  m_closeButton = new QPushButton("Close");
  m_intensitySlider = new QSlider( Qt::Horizontal);
  m_intensitySlider->setMinimum(0);
  m_intensitySlider->setMaximum(10);
  m_intensitySlider->setValue(5);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget( m_colorButton );
  mainLayout->addWidget( m_intensitySlider);
  connect(m_colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
  connect(m_intensitySlider, SIGNAL(valueChanged(int)), this, SLOT(updateLightProperties()));
  connect(m_closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

// ***********

void SGLightManagerDialog::showColorDialog() {
  if(SGBaseManagerDialog::showColorDialog())
    updateLightProperties();
}

void SGLightManagerDialog::updateLightProperties() {
  // Here we should be able to dynamically update the scene light
   FABRIC_TRY("SGLightManagerDialog::updateLightProperties",
    // Get the click position
    std::vector<FabricCore::RTVal> klParams(2); 
    klParams[0] = m_color;
    klParams[1] = FabricCore::RTVal::ConstructFloat32(*m_client, float(m_intensitySlider->value()/10.0f));;
    m_testObject.callMethod("", "onSetlightProperties", 2, &klParams[0]); 
    m_parent->update();
  );
}
