#ifndef __IMAGEVIEWPORT_H__
#define __IMAGEVIEWPORT_H__


#include <QtCore/QtCore>
#include <QtCore/QTime>
#include <QtGui/QMouseEvent>
#include <QtGui/QImage>
#include <QtOpenGL/QGLWidget>

#include <FabricCore.h>

namespace FabricUI
{
  namespace Viewports
  {
    class ImageViewportWidget : public QGLWidget
    {
    	Q_OBJECT

    public:

    	ImageViewportWidget(FabricCore::Client *client, QString argumentName, QWidget *parent = NULL);
    	virtual ~ImageViewportWidget();

      void setBinding(FabricCore::DFGBinding binding);

      double fps() const { return m_fps; }

    protected:

      virtual void initializeGL();
      virtual void resizeGL(int w, int h);
      virtual void paintGL();

      void mousePressEvent(QMouseEvent *event);
      void mouseMoveEvent(QMouseEvent *event);
      void mouseReleaseEvent(QMouseEvent *event);
      void wheelEvent(QWheelEvent *event);

      int m_width;
      int m_height;
      FabricCore::Client *m_client;
      QString m_argumentName;
      FabricCore::RTVal m_imageSeq;
      FabricCore::RTVal m_workData;
      FabricCore::DFGBinding m_binding;

      QTime m_fpsTimer;
      double m_fps;
      double m_fpsStack[16];

    };
  };
};

#endif // __IMAGEVIEWPORT_H__
