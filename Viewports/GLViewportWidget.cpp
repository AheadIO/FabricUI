#include "GLViewportWidget.h"
#include "QtToKLEvent.h"

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

GLViewportWidget::GLViewportWidget(FabricCore::Client * client, QColor bgColor, QGLFormat format, QWidget *parent)
: QGLWidget(format, parent)
{	
  m_client = client;
  m_bgColor = bgColor;
	setFocusPolicy(Qt::StrongFocus);
  setAutoBufferSwap(false);

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
  resetRTVals();
}

GLViewportWidget::~GLViewportWidget()
{
}

QColor GLViewportWidget::backgroundColor() const
{
  return m_bgColor;
}

void GLViewportWidget::setBackgroundColor(QColor color)
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

void GLViewportWidget::clearInlineDrawing()
{
  if(m_viewport.isValid() && m_drawContext.isValid())
  {
    try
    {
      FabricCore::RTVal oglInlineDrawing = FabricCore::RTVal::Create(*m_client, "OGLInlineDrawing", 0, 0);
      oglInlineDrawing = oglInlineDrawing.callMethod("OGLInlineDrawing", "getInstance", 0, 0);
      FabricCore::RTVal rootTransform = oglInlineDrawing.callMethod("InlineTransform", "getRoot", 0, 0);
      rootTransform.callMethod("", "removeAllChildren", 0, 0);
      m_viewport.callMethod("", "setup", 1, &m_drawContext);
    }
    catch(FabricCore::Exception e)
    {
      printf("Error: %s\n", e.getDesc_cstr());
    }
  }
}

void GLViewportWidget::redraw()
{
  updateGL();
}

void GLViewportWidget::initializeGL()
{
  try
  {
    m_viewport.callMethod("", "setup", 1, &m_drawContext);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
}

void GLViewportWidget::resizeGL(int width, int height)
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

void GLViewportWidget::paintGL()
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

void GLViewportWidget::resetRTVals()
{
  try
  {
    m_drawing = FabricCore::RTVal::Create(*m_client, "OGLInlineDrawing", 0, 0);
    if(!m_drawing.isValid())
    {
      printf("[GLWidget] Error: Cannot construct OGLInlineDrawing RTVal (extension loaded?)\n");
      return;
    }
    m_drawing = m_drawing.callMethod("OGLInlineDrawing", "getInstance", 0, 0);

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
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
}

void GLViewportWidget::mousePressEvent(QMouseEvent *event)
{
  if(!manipulateCamera(event))
      QGLWidget::mousePressEvent(event);
}

void GLViewportWidget::mouseMoveEvent(QMouseEvent *event)
{
  if(!manipulateCamera(event))
    QGLWidget::mouseMoveEvent(event);
}

void GLViewportWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if(!manipulateCamera(event))
    QGLWidget::mouseReleaseEvent(event);
}

void GLViewportWidget::wheelEvent(QWheelEvent *event)
{
  if(!manipulateCamera(event))
    QGLWidget::wheelEvent(event);
}

bool GLViewportWidget::manipulateCamera(QInputEvent *event)
{
  if(!event->modifiers().testFlag(Qt::AltModifier))
    return false;

  bool result;
  try
  {
    // Now we translate the Qt events to FabricEngine events..
    FabricCore::RTVal klevent = QtToKLEvent(event, m_client, m_viewport);

    // And then pass the event to the camera manipulator for handling.
    m_cameraManipulator.callMethod("", "onEvent", 1, &klevent);
    result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
  updateGL();
  return result;
}
