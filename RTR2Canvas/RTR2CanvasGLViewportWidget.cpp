#include "RTR2CanvasGLViewportWidget.h"
#include <FabricUI/Util/QtToKLEvent.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <ostream>
#include <fstream>
#include <streambuf>
#include <memory>

using namespace FabricUI::Viewports;


RTR2CanvasGLViewportWidget::RTR2CanvasGLViewportWidget(FabricCore::Client * client, QColor bgColor, QGLFormat format, QWidget *parent, QSettings *settings)
: QGLWidget(format, parent)
, m_settings(settings)
{	
  m_client = client;
  m_bgColor = bgColor;
  m_usingStage = true;
  m_stageVisible = true;
  m_hasCommercialLicense = client->hasCommercialLicense();
  setFocusPolicy(Qt::StrongFocus);
  setAutoBufferSwap(false);

  if(m_settings)
  {
    if(m_settings->contains("glviewport/usingStage"))
      m_usingStage = m_settings->value("glviewport/usingStage").toBool();
    if(m_settings->contains("glviewport/stageVisible"))
      m_stageVisible = m_settings->value("glviewport/stageVisible").toBool();
  }

  try
  {
    m_client->loadExtension("Manipulation", "", false);
    m_client->loadExtension("InlineDrawing", "", false);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }

  m_fps = 0.0;

  for(int i=0;i<16;i++)
    m_fpsStack[i] = 0.0;

  m_fpsTimer.start();
  m_resizedOnce = false;
  resetRTVals( false /*shouldUpdateGL*/ );
}

RTR2CanvasGLViewportWidget::~RTR2CanvasGLViewportWidget()
{
}

QColor RTR2CanvasGLViewportWidget::backgroundColor() const
{
  return m_bgColor;
}

void RTR2CanvasGLViewportWidget::setBackgroundColor(QColor color)
{
  m_bgColor = color;

  if(m_viewport.isValid())
  {
    try
    {
      FabricCore::RTVal bgColorVal = m_viewport.maybeGetMember("bgColor");
      bgColorVal.setMember("r", FabricCore::RTVal::ConstructFloat32(*m_client, float(m_bgColor.red()) / 255.0f));
      bgColorVal.setMember("g", FabricCore::RTVal::ConstructFloat32(*m_client, float(m_bgColor.green()) / 255.0f));
      bgColorVal.setMember("b", FabricCore::RTVal::ConstructFloat32(*m_client, float(m_bgColor.blue()) / 255.0f));
      m_viewport.setMember("bgColor", bgColorVal);
    }
    catch(FabricCore::Exception e)
    {
      printf("Error: %s\n", e.getDesc_cstr());
    }
  }
}

void RTR2CanvasGLViewportWidget::clearInlineDrawing()
{
  if(m_viewport.isValid() && m_drawContext.isValid())
  {
    resetRTVals();
    initializeGL();
    resizeGL(size().width(), size().height());
  }
}

void RTR2CanvasGLViewportWidget::redraw()
{
  updateGL();
}

void RTR2CanvasGLViewportWidget::onKeyPressed(QKeyEvent * event)
{
  keyPressEvent(event);  
}

void RTR2CanvasGLViewportWidget::initializeGL()
{
  try
  {
    m_viewport.callMethod("", "setup", 1, &m_drawContext);
    m_drawing = m_drawing.callMethod("OGLInlineDrawing", "getInstance", 0, 0);
    setStageVisible(m_stageVisible, false);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
}

void RTR2CanvasGLViewportWidget::resizeGL(int width, int height)
{
  m_width = width;
  m_height = height;

  try
  {
    FabricCore::RTVal args[3];
    args[0] = m_drawContext;
    args[1] = FabricCore::RTVal::ConstructUInt32(*m_client, (unsigned int)m_width);
    args[2] = FabricCore::RTVal::ConstructUInt32(*m_client, (unsigned int)m_height);
    m_viewport.callMethod("", "resize", 3, args);
    m_resizedOnce = true;
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
}

void RTR2CanvasGLViewportWidget::paintGL()
{
  if(!m_resizedOnce)
  {
    QSize scale = size();
    resizeGL(scale.width(), scale.height());
  }

  // compute the fps
  double ms = m_fpsTimer.elapsed();
  if(ms == 0.0)
    m_fps = 0.0;
  else
    m_fps = 1000.0 / ms;

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

  try
  {
    FabricCore::RTVal args[2];
    args[0] = FabricCore::RTVal::ConstructString(*m_client, "default");
    args[1] = m_drawContext;
    m_drawing.callMethod("", "drawViewport", 2, args);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }

  swapBuffers();
  emit redrawn();
}

void RTR2CanvasGLViewportWidget::resetRTVals( bool shouldUpdateGL )
{
  try
  {
    m_drawing = FabricCore::RTVal::Create(*m_client, "OGLInlineDrawing", 0, 0);
    if(!m_drawing.isValid())
    {
      printf("[GLWidget] Error: Cannot construct OGLInlineDrawing RTVal (extension loaded?)\n");
      return;
    }
    m_drawing = m_drawing.callMethod("OGLInlineDrawing", "getNewInstance", 0, 0);

    m_viewport = FabricCore::RTVal::Create(*m_client, "OGLStandaloneViewport", 0, 0);
    if(!m_viewport.isValid())
    {
      printf("[GLWidget] Error: Cannot construct OGLStandaloneViewport RTVal (extension loaded?)\n");
      return;
    }
    else
    {
      FabricCore::RTVal args[2];
      args[0] = FabricCore::RTVal::ConstructString(*m_client, "default");
      args[1] = m_viewport;
      m_drawing.callMethod("", "registerViewport", 2, args);

      // [pzion 20150909] No viewport overlay, at least for now
      // if(!m_hasCommercialLicense)
      //   m_viewport.callMethod("", "setupLicensingOverlay", 0, 0);
    }

    m_camera = m_viewport.maybeGetMember("camera");
    m_cameraManipulator = FabricCore::RTVal::Create(*m_client, "CameraManipulator", 1, &m_camera);

    m_viewport.setMember("windowId", FabricCore::RTVal::ConstructUInt64(*m_client, (uint64_t)this->winId()));

    setBackgroundColor(m_bgColor);

    m_drawContext = FabricCore::RTVal::Create(*m_client, "DrawContext", 0, 0);
    if(!m_drawContext.isValid())
    {
      printf("[GLWidget] Error: Cannot construct DrawContext RTVal (extension loaded?)\n");
      return;
    }
    m_drawContext = m_drawContext.callMethod("DrawContext", "getInstance", 0, 0);
    m_drawContext.callMethod("", "resetAllViewportParams", 0, 0);

    // [pzion 20150728] Artificially push a meaningless movement
    // event to force camera manipulation code to compiler, preventing
    // annoying pauses
    QMouseEvent nullEvent(
      QMouseEvent::MouseMove,
      QCursor::pos(),
      Qt::NoButton,
      Qt::NoButton,
      Qt::NoModifier
      );
    
    manipulateCamera(
      &nullEvent,
      false, // requireModifier
      false // shouldUpdateGL
      );
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }

  setUsingStage( m_usingStage, shouldUpdateGL );
  setStageVisible( m_stageVisible, shouldUpdateGL );
}

void RTR2CanvasGLViewportWidget::mousePressEvent(QMouseEvent *event)
{
  if(manipulateCamera(event))
    return;
  QGLWidget::mousePressEvent(event);
}

void RTR2CanvasGLViewportWidget::mouseMoveEvent(QMouseEvent *event)
{
  if(manipulateCamera(event))
    return;
  QGLWidget::mouseMoveEvent(event);
}

void RTR2CanvasGLViewportWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if(manipulateCamera(event))
    return;
  QGLWidget::mouseReleaseEvent(event);
}

void RTR2CanvasGLViewportWidget::wheelEvent(QWheelEvent *event)
{
  if(manipulateCamera(event, false))//m_manipTool->isActive()))
    return;
  QGLWidget::wheelEvent(event);
}

bool RTR2CanvasGLViewportWidget::manipulateCamera(
  QInputEvent *event,
  bool requireModifier,
  bool shouldUpdateGL
  )
{
  if(!event->modifiers().testFlag(Qt::AltModifier) && requireModifier)
    return false;

  bool result = false;
  try
  {
    // Now we translate the Qt events to FabricEngine events..
    FabricCore::RTVal klevent = QtToKLEvent(event, *m_client, m_viewport);

    // And then pass the event to the camera manipulator for handling.
    m_cameraManipulator.callMethod("", "onEvent", 1, &klevent);
    result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
  if ( shouldUpdateGL )
    updateGL();
  return result;
}

bool RTR2CanvasGLViewportWidget::isUsingStage()
{
  return m_usingStage;
}

bool RTR2CanvasGLViewportWidget::isStageVisible()
{
  return m_stageVisible;
}

void RTR2CanvasGLViewportWidget::setUsingStage(
  bool usingStage,
  bool update
  )
{
  m_usingStage = usingStage;
  if(m_settings)
  {
    m_settings->setValue("glviewport/usingStage", m_usingStage);
  }

  if(!m_viewport.isValid())
    return;

  try
  {
    FabricCore::RTVal usingStageVal = FabricCore::RTVal::ConstructBoolean(*m_client, m_usingStage);
    m_viewport.callMethod("", "setUsingStage", 1, &usingStageVal);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
  if(update)
    updateGL();
}

void RTR2CanvasGLViewportWidget::setStageVisible( bool stageVisible, bool update )
{
  m_stageVisible = stageVisible;
  if(m_settings)
  {
    m_settings->setValue("glviewport/stageVisible", m_stageVisible);
  }

  if(!m_viewport.isValid())
    return;

  try
  {
    FabricCore::RTVal stageVisibleVal = FabricCore::RTVal::ConstructBoolean(*m_client, m_stageVisible);
    m_viewport.callMethod("", "setStageVisible", 1, &stageVisibleVal);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }

  if(update)
    updateGL();
}

void RTR2CanvasGLViewportWidget::resetCamera()
{
  if(!m_viewport.isValid())
    return;

  try
  {
    FabricCore::RTVal position[3];
    position[0] = FabricCore::RTVal::ConstructFloat32(*m_client, 30.0f);
    position[1] = FabricCore::RTVal::ConstructFloat32(*m_client, 20.0f);
    position[2] = FabricCore::RTVal::ConstructFloat32(*m_client, 40.0f);
    
    FabricCore::RTVal target[3];
    target[0] = FabricCore::RTVal::ConstructFloat32(*m_client, 0.0f);
    target[1] = FabricCore::RTVal::ConstructFloat32(*m_client, 0.0f);
    target[2] = FabricCore::RTVal::ConstructFloat32(*m_client, 0.0f);
    
    FabricCore::RTVal args[2];
    args[0] = FabricCore::RTVal::Construct(*m_client, "Vec3", 3, position);
    args[1] = FabricCore::RTVal::Construct(*m_client, "Vec3", 3, target);

    m_camera.callMethod("", "setFromPositionAndTarget", 2, args);
    m_cameraManipulator = FabricCore::RTVal::Create(*m_client, "CameraManipulator", 1, &m_camera);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }

  updateGL();
}

