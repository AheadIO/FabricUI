/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHGeometryEditorDialog.h"
#include <QtGui/QLayout>
using namespace FabricUI::SceneHub;


SHGeometryEditorDialog::SHGeometryEditorDialog(QWidget *parent, SHGLScene *shGLScene)
  : SHBaseEditorDialog(parent, shGLScene)
{
  m_colorButton = new QPushButton("Color");
  m_closeButton = new QPushButton("Close");
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(m_colorButton);
  mainLayout->addWidget(m_closeButton);
  connect(m_colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
  connect(m_closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

void SHGeometryEditorDialog::showColorDialog() {
  SHBaseEditorDialog::showColorDialog();
  updatetGeometryProperties();
}

void SHGeometryEditorDialog::updatetGeometryProperties() {
  // !! m_shGLScene->setGeometryProperties(m_color);
  // !! m_parent->update();
}



