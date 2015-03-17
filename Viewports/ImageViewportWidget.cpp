#include "ImageViewportWidget.h"

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

ImageViewportWidget::ImageViewportWidget(FabricCore::Client * client, QString argumentName, QWidget *parent)
:	QGLWidget(parent)
{	
  // setAutoBufferSwap(false);
  m_client = client;
  m_argumentName = argumentName;
	setFocusPolicy(Qt::StrongFocus);

  m_fps = 0.0;

  for(int i=0;i<16;i++)
    m_fpsStack[i] = 0.0;

  m_fpsTimer.start();
}

ImageViewportWidget::~ImageViewportWidget()
{
}

void ImageViewportWidget::setBinding(FabricServices::DFGWrapper::Binding binding)
{
  m_binding = binding;
  m_imageSeq = FabricCore::RTVal();
  m_workData = FabricCore::RTVal();
}

void ImageViewportWidget::initializeGL()
{
  glClearColor(0.34, 0.34, 0.34, 1.0);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_TEXTURE_2D);
}

void ImageViewportWidget::resizeGL(int width, int height)
{
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, height, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  m_width = width;
  m_height = height;

  try
  {
    if(!m_workData.isValid())
      m_workData = FabricCore::RTVal::Create(*m_client, "ImageSeqWorkData", 0, 0);
    m_workData.setMember("width", FabricCore::RTVal::ConstructSInt32(*m_client, m_width));
    m_workData.setMember("height", FabricCore::RTVal::ConstructSInt32(*m_client, m_height));
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
    return;
  }
}

void ImageViewportWidget::paintGL()
{
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

  glClear(GL_COLOR_BUFFER_BIT);

  if(!m_binding.isValid())
    return;

  if(!m_imageSeq.isValid())
  {
    try
    {
      m_imageSeq = m_binding.getArgValue(m_argumentName.toUtf8().constData());
      if(m_imageSeq.isValid())
      {
        if(m_imageSeq.isNullObject())
        {
          m_imageSeq = FabricCore::RTVal();
          return;
        }
      }
    }
    catch(FabricCore::Exception e)
    {
      printf("Exception: %s\n", e.getDesc_cstr());
      return;
    }
  }

  try
  {
    if(!m_workData.isValid())
    {
      m_workData = FabricCore::RTVal::Create(*m_client, "ImageSeqWorkData", 0, 0);
      m_workData.setMember("width", FabricCore::RTVal::ConstructSInt32(*m_client, width()));
      m_workData.setMember("height", FabricCore::RTVal::ConstructSInt32(*m_client, height()));
    }
    m_imageSeq.callMethod("", "draw", 1, &m_workData);
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
    return;
  }
}

void ImageViewportWidget::mousePressEvent(QMouseEvent *event)
{
  QGLWidget::mousePressEvent(event);
}

void ImageViewportWidget::mouseMoveEvent(QMouseEvent *event)
{
  QGLWidget::mouseMoveEvent(event);
}

void ImageViewportWidget::mouseReleaseEvent(QMouseEvent *event)
{
  QGLWidget::mouseReleaseEvent(event);
}

void ImageViewportWidget::wheelEvent(QWheelEvent *event)
{
  QGLWidget::wheelEvent(event);
}
