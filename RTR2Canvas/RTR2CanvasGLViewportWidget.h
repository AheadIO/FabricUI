#ifndef __RTR2Canvas_GLVIEWPORT_H__
#define __RTR2Canvas_GLVIEWPORT_H__


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
    class RTR2CanvasGLViewportWidget : public QGLWidget
    {
    	Q_OBJECT

      friend class MainWindow;

    public:

    	RTR2CanvasGLViewportWidget(FabricCore::Client * client, QColor bgColor, QGLFormat format, QWidget *parent = NULL, QSettings *settings = NULL);
    	virtual ~RTR2CanvasGLViewportWidget();

      double fps() const { return m_fps; }

      QColor backgroundColor() const;
      void setBackgroundColor(QColor color);

      void clearInlineDrawing();

      FabricCore::Client * getClient() { return m_client; }
      FabricCore::RTVal getCamera() const { return m_camera; }
      FabricCore::RTVal getCameraManipulator() const { return m_cameraManipulator; }
      FabricCore::RTVal getViewport() const { return m_viewport; }
      FabricCore::RTVal getDrawContext() const { return m_drawContext; }

      bool isUsingStage();
      bool isStageVisible();

    public slots:

      void redraw();
      void onKeyPressed(QKeyEvent * event);
      void setUsingStage( bool usingStage, bool update = true );
      void setStageVisible( bool stageVisible, bool update = true );
      void resetCamera();

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
      bool m_hasCommercialLicense;
      QColor m_bgColor;
      FabricCore::Client * m_client;
      FabricCore::RTVal m_drawing;
      FabricCore::RTVal m_camera;
      FabricCore::RTVal m_cameraManipulator;
      FabricCore::RTVal m_viewport;
      FabricCore::RTVal m_drawContext;

      QTime m_fpsTimer;
      double m_fps;
      double m_fpsStack[16];

      QSettings * m_settings;

    };
  };
};

#endif // __RTR2Canvas_GLVIEWPORT_H__
