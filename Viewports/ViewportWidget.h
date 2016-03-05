/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_VIEWPORT_H__
#define __FABRICUI_VIEWPORT_H__

#include <FabricUI/Viewports/QtToKLEvent.h>
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

#include <QtCore/QtCore>
#include <QtCore/QTime>
#include <QtCore/QSettings>
#include <QtGui/QMouseEvent>
#include <QtGui/QImage>
#include <QtOpenGL/QGLWidget>

#include <FabricCore.h>

namespace FabricUI
{
  namespace Viewports
  {
    /// Defines in Canvas, not SceneHub
    class ManipulationTool;

    class ViewportWidget : public QGLWidget
    { 
      private:
        void init(FabricCore::Client client, QColor bgColor, QSettings *settings = 0);

      public:
        /// Constructor for RTRGLViewportWidget.
        ViewportWidget(
          FabricCore::Client client, 
          QColor bgColor, 
          QGLContext *qglContext, 
          QWidget *parent = 0, 
          QGLWidget *share = 0,
          QSettings *settings = 0);

        /// Constructor for GLViewportWidget.
      	ViewportWidget(
          FabricCore::Client client, 
          QColor bgColor, 
          QGLFormat format, 
          QWidget *parent = 0, 
          QSettings *settings = 0);

      	virtual ~ViewportWidget() {};

        double fps() const { return m_fps; }
        QColor backgroundColor() const {return m_bgColor; };
      
        FabricCore::Client getClient() { return m_client; }
        FabricCore::RTVal getViewport() const { return m_viewport; }
        virtual FabricCore::RTVal getCamera() = 0;
        virtual void setBackgroundColor(QColor color) = 0;
      
        // Canvas (InlineDrawing) specific
        virtual void clearInlineDrawing() {};
        virtual bool isManipulationActive() const { return false; };
        virtual void setManipulationActive(bool state) {};
        virtual ManipulationTool *getManipTool() { return 0; };
        virtual bool isUsingStage() { return false; };
        virtual bool isStageVisible() { return false; };


      public slots:
        virtual void redraw() { updateGL(); };
        virtual void onKeyPressed(QKeyEvent * event) {};
        virtual void onContextMenu(const QPoint &point) {};
        

      protected:
        virtual void keyPressEvent(QKeyEvent * event);
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);
        virtual void mouseReleaseEvent(QMouseEvent *event);
        virtual void wheelEvent(QWheelEvent *event) ;
        void computeFPS();
       

        double m_fps;
        bool m_hasCommercialLicense;
        double m_fpsStack[16];
        FabricCore::Client m_client;
     
        QColor m_bgColor;
        QTime m_fpsTimer;
        QSettings *m_settings;
        FabricCore::RTVal m_viewport;
    };
  };
};

#endif // __FABRICUI_VIEWPORT_H__
