/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHBaseEditorDialog.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QColorDialog>
#include <QtCore/QCoreApplication>
using namespace FabricUI::SceneHub;

SHBaseEditorDialog::SHBaseEditorDialog(QWidget*parent, SHGLScene *shGLScene) 
  : QWidget(parent)
  , m_shGLScene(shGLScene)
{
  setMouseTracking(true);
  setWindowTitle("");
  setMouseTracking(true);
  setFixedSize(400, 400);
  if(parent) move(parent->mapToGlobal(parent->rect().center()));
  setWindowFlags(Qt::WindowStaysOnTopHint);
}
 
void SHBaseEditorDialog::showColorDialog() {
  QColor color = QColorDialog::getColor(Qt::yellow, this);
  if(color == QColor::Invalid) return;
  m_color[0] = color.redF();
  m_color[1] = color.greenF();
  m_color[2] = color.blueF();
  m_color[3] = color.alphaF();
}

void SHBaseEditorDialog::closeEvent(QCloseEvent *e) {
  e->accept();
  if(!parentWidget()) return;
  QMouseEvent me(QEvent::MouseButtonRelease, QPoint(0,0), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
  QCoreApplication::sendEvent(parentWidget(), &me);
}

 
