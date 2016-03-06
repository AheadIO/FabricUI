/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/Viewports/ViewportWidget.h>

using namespace FabricUI::Viewports;

void ViewportWidget::init(FabricCore::Client *client, QColor bgColor, QSettings *settings) { 
  m_settings = settings;
  m_client = client;
  m_bgColor = bgColor;
 
  m_hasCommercialLicense = m_client->hasCommercialLicense();
  setFocusPolicy(Qt::StrongFocus);
 
  m_fps = 0.0;
  for(int i=0;i<16;i++) m_fpsStack[i] = 0.0;
}

ViewportWidget::ViewportWidget(
  FabricCore::Client *client, 
  QColor bgColor, 
  QGLContext *qglContext, 
  QWidget *parent, 
  QGLWidget *share,
  QSettings *settings)
  : QGLWidget(qglContext, parent, share)
{ 
  init(client, bgColor, settings);
}

ViewportWidget::ViewportWidget(
  FabricCore::Client *client, 
  QColor bgColor, 
  QGLFormat format, 
  QWidget *parent, 
  QSettings *settings)
  : QGLWidget(format, parent)
{	
  init(client, bgColor, settings);
  setAutoBufferSwap(false);
}
 
void ViewportWidget::computeFPS() {
   
  // compute the fps
  double ms = m_fpsTimer.elapsed();
  m_fps = (ms == 0.0) ? 0.0 : 1000.0 / ms;
  double averageFps = 0.0;
  for(int i=0;i<15;i++) {
    m_fpsStack[i+1] = m_fpsStack[i];
    averageFps += m_fpsStack[i];
  }
  m_fpsStack[0] = m_fps;
  averageFps += m_fps;
  averageFps /= 16.0;
  m_fps = averageFps;
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
