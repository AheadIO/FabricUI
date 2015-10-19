// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_VIEWPORTS_RTRGLVIEWPORTWIDGET__
#define __FABRICUI_VIEWPORTS_RTRGLVIEWPORTWIDGET__

// #include <GL/glew.h>

#include <QtCore/QtCore>
#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>
#include <QtGui/QTextEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QImage>
#include <QtGui/QDialog>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QTime>
#include <QtGui/QMenuBar>
#include <FabricCore.h>
#include <FabricUI/SceneHub/SGLightManagerDialog.h>
#include <FabricUI/SceneHub/SGGeometryManagerDialog.h>

namespace FabricUI
{
  namespace Viewports
  {
    class RTRGLViewportWidget : public QGLWidget
    {
      Q_OBJECT

      public:
        RTRGLViewportWidget(FabricCore::Client *, FabricCore::RTVal, int, QGLContext *, QWidget *parent = NULL, QGLWidget *shared = NULL);
        virtual ~RTRGLViewportWidget();

        void initialize();
        void resetRTVals();
        void setTime(float time);
        void toggleAlwaysRefresh();
        bool alwaysRefreshes() { return m_alwaysRefresh; }
        double fps() const { return m_fps; }

      signals:
        void onDrop();
        void manipsAcceptedEvent();
        void viewportDestroying();

      public slots:
        void onContextMenu(const QPoint &point);
        void editObjectColor();
        void editLocalObjectColor();

        void editLightProperties();
        void addArchive();
        void addLight();
        void addTexture();

      private:
        void constuctAddMenu();
        void constuctLightMenu();
        void constuctGeometryMenu(std::string category);
        void addExternalFile(QStringList, QPoint, bool);
        void editObjectColor( bool local );
      
      protected:
        virtual void paintGL();
        virtual void resizeGL(int w, int h);

        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);
        bool onMouseEvent(QEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void dragEnterEvent(QDragEnterEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void dropEvent(QDropEvent *event);

        QWidget *m_parent;
        FabricCore::Client *m_client;

        int m_viewportIndex;

        FabricCore::RTVal m_viewport;
        FabricCore::RTVal m_testObject;
        FabricCore::RTVal m_viewportIndexRTVal;
        FabricCore::RTVal m_width;
        FabricCore::RTVal m_height;

        QPoint m_screenPos;
        QTime m_fpsTimer;
        double m_fps;
        double m_fpsStack[16];
        bool m_alwaysRefresh;

        FabricUI::SceneHub::SGBaseManagerDialog *m_geometryDialog;
        FabricUI::SceneHub::SGLightManagerDialog *m_lightDialog;
    };
  };
};

#endif // __FABRICUI_VIEWPORTS_RTRGLVIEWPORTWIDGET__
