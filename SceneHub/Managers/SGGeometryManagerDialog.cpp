/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/SceneHub/Managers/SGGeometryManagerDialog.h>
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


SGGeometryManagerDialog::SGGeometryManagerDialog(QWidget* parent, FabricCore::Client client, FabricCore::RTVal testObject) {
  init(parent, client, testObject);
  m_colorButton = new QPushButton("Color");
  m_closeButton = new QPushButton("Close");
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget( m_colorButton );
  mainLayout->addWidget( m_closeButton );

  connect(m_colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
  connect(m_closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

// ***********

void SGGeometryManagerDialog::showColorDialog() {
  SGBaseManagerDialog::showColorDialog();
  updatetGeometryProperties();
}

void SGGeometryManagerDialog::updatetGeometryProperties() {
  FABRIC_TRY("GLWidget::setGeometryProperties",
    m_testObject.callMethod("", "onSetGeometryProperties", 1, &m_color); 
  );
}



