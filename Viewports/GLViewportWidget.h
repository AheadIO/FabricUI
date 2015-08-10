#ifndef __GLVIEWPORT_H__
#define __GLVIEWPORT_H__


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
    class ManipulationTool;

    class GLViewportWidget : public QGLWidget
    {
    	Q_OBJECT

      friend class MainWindow;
      friend class ManipulationTool;

    public:

    	GLViewportWidget(FabricCore::Client * client, QColor bgColor, QGLFormat format, QWidget *parent = NULL, QSettings *settings = NULL);
    	virtual ~GLViewportWidget();

      double fps() const { return m_fps; }

      QColor backgroundColor() const;
      void setBackgroundColor(QColor color);

      bool isManipulationActive() const;
      void setManipulationActive(bool state);

      void clearInlineDrawing();

      FabricCore::Client * getClient() { return m_client; }
      FabricCore::RTVal getCamera() const { return m_camera; }
      FabricCore::RTVal getCameraManipulator() const { return m_cameraManipulator; }
      FabricCore::RTVal getViewport() const { return m_viewport; }
      FabricCore::RTVal getDrawContext() const { return m_drawContext; }
      ManipulationTool * getManipTool() { return m_manipTool; }

      bool isUsingStage();
      bool isStageVisible();

    public slots:

      void redraw();
      void onKeyPressed(QKeyEvent * event);
      void toggleManipulation() { setManipulationActive(!isManipulationActive()); }
      void setUsingStage( bool usingStage, bool update = true );
      void setStageVisible( bool stageVisible, bool update = true );

    signals:

      void redrawn();
      void portManipulationRequested(QString portName);

    protected:

      virtual void initializeGL();
      virtual void resizeGL(int w, int h);
      virtual void paintGL();

      void resetRTVals( bool shouldUpdateGL = true );
      bool manipulateCamera(
        QInputEvent *event,
        bool requireModifier = true,
        bool shouldUpdateGL = true
        );

      void mousePressEvent(QMouseEvent *event);
      void mouseMoveEvent(QMouseEvent *event);
      void mouseReleaseEvent(QMouseEvent *event);
      void wheelEvent(QWheelEvent *event);

      int m_width;
      int m_height;
      bool m_resizedOnce;
      bool m_stageVisible;
      bool m_usingStage;
      QColor m_bgColor;
      FabricCore::Client * m_client;
      FabricCore::RTVal m_drawing;
      FabricCore::RTVal m_camera;
      FabricCore::RTVal m_cameraManipulator;
      FabricCore::RTVal m_viewport;
      FabricCore::RTVal m_drawContext;

      ManipulationTool * m_manipTool;

      QTime m_fpsTimer;
      double m_fps;
      double m_fpsStack[16];

      QSettings * m_settings;

    };
  };
};

#include "ManipulationTool.h"

#endif // __GLVIEWPORT_H__
