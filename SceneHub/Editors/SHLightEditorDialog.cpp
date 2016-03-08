/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHLightEditorDialog.h"
#include <QtGui/QLayout>
using namespace FabricUI::SceneHub;


SHLightEditorDialog::SHLightEditorDialog(QWidget *parent, SHGLScene *shGLScene)
  : SHBaseEditorDialog(parent, shGLScene)
{
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

void SHLightEditorDialog::showColorDialog() {
  SHBaseEditorDialog::showColorDialog();
  updateLightProperties();
}

void SHLightEditorDialog::updateLightProperties() {
  // Here we should be able to dynamically update the scene light
  m_shGLScene->setlightProperties(m_color, m_intensitySlider->value()/10.0f);
}
