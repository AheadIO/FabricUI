/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/Viewports/ViewportWidget.h>

using namespace FabricUI::Viewports;


ViewportWidget::ViewportWidget(
  FabricCore::Client *client, 
  QColor bgColor, 
  QGLContext *qglContext, 
  QWidget *parent, 
  QGLWidget *share,
  QSettings *settings)
  : QGLWidget(qglContext, parent, share)
  , m_settings(settings)
{ 
  m_client = client;
  m_bgColor = bgColor;
 
  m_hasCommercialLicense = client->hasCommercialLicense();
  setFocusPolicy(Qt::StrongFocus);
  //setAutoBufferSwap(false);
 
  m_fps = 0.0;
  for(int i=0;i<16;i++) m_fpsStack[i] = 0.0;
  m_fpsTimer.start();
}

ViewportWidget::ViewportWidget(
  FabricCore::Client *client, 
  QColor bgColor, 
  QGLFormat format, 
  QWidget *parent, 
  QSettings *settings)
  : QGLWidget(format, parent)
  , m_settings(settings)
{	
  m_client = client;
  m_bgColor = bgColor;
 
  m_hasCommercialLicense = client->hasCommercialLicense();
  setFocusPolicy(Qt::StrongFocus);
  setAutoBufferSwap(false);
 
  m_fps = 0.0;
  for(int i=0;i<16;i++) m_fpsStack[i] = 0.0;
  m_fpsTimer.start();
}
 
void ViewportWidget::keyPressEvent(QKeyEvent * event) {
  QGLWidget::keyPressEvent(event);
}

void ViewportWidget::mousePressEvent(QMouseEvent *event) {
  QGLWidget::mousePressEvent(event);
}

void ViewportWidget::mouseMoveEvent(QMouseEvent *event) {
  QGLWidget::mouseMoveEvent(event);
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent *event) {
  QGLWidget::mouseReleaseEvent(event);
}

void ViewportWidget::wheelEvent(QWheelEvent *event) {
  QGLWidget::wheelEvent(event);
}
 