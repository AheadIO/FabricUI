#ifndef __GLVIEWPORT_H__
#define __GLVIEWPORT_H__


#include <QtCore/QtCore>
#include <QtCore/QTime>
#include <QtGui/QMouseEvent>
#include <QtGui/QImage>
#include <QtOpenGL/QGLWidget>

#include <FabricCore.h>
#include <DFGWrapper/Binding.h>

namespace FabricUI
{
  namespace Viewports
  {
    class GLViewportWidget : public QGLWidget
    {
    	Q_OBJECT

      friend class MainWindow;

    public:

    	GLViewportWidget(FabricCore::Client * client, QColor bgColor, QGLFormat format, QWidget *parent = NULL);
    	virtual ~GLViewportWidget();

      double fps() const { return m_fps; }

      QColor backgroundColor() const;
      void setBackgroundColor(QColor color);

      void clearInlineDrawing();

      FabricCore::RTVal getCamera() const { return m_camera; }
      FabricCore::RTVal getCameraManipulator() const { return m_cameraManipulator; }
      FabricCore::RTVal getViewport() const { return m_viewport; }
      FabricCore::RTVal getDrawContext() const { return m_drawContext; }

    public slots:

      void redraw();
      void onKeyPressed(QKeyEvent * event);

    signals:

      void redrawn();

    protected:

      virtual void initializeGL();
      virtual void resizeGL(int w, int h);
      virtual void paintGL();

      void resetRTVals();
      bool manipulateCamera(QInputEvent *event);

      void mousePressEvent(QMouseEvent *event);
      void mouseMoveEvent(QMouseEvent *event);
      void mouseReleaseEvent(QMouseEvent *event);
      void wheelEvent(QWheelEvent *event);

      int m_width;
      int m_height;
      bool m_resizedOnce;
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

    };
  };
};

#endif // __GLVIEWPORT_H__
