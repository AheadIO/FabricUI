/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SCENEHUB_RTRGLVIEWPORTWIDGET_H__
#define __FABRICUI_SCENEHUB_RTRGLVIEWPORTWIDGET_H__

#include <QtCore/QTime>
#include <QtCore/QtCore>
#include <QtGui/QDrag>
#include <QtGui/QImage>
#include <QtGui/QDialog>
#include <QtGui/QMenuBar>
#include <QtGui/QTextEdit>
#include <QtGui/QMouseEvent>
#include <QtGui/QMainWindow>
#include <QtOpenGL/QGLWidget>
#include <FabricCore.h>
#include <FabricUI/Viewports/ViewportWidget.h>
#include <FabricUI/SceneHub/Managers/SGLightManagerDialog.h>
#include <FabricUI/SceneHub/Managers/SGGeometryManagerDialog.h>
#include <FabricUI/SceneHub/Commands/SHCmdView.h>


namespace FabricUI
{
  namespace Viewports
  {
    class RTRGLViewportWidget : public ViewportWidget
    {
      Q_OBJECT

      public:
        RTRGLViewportWidget(
          FabricCore::Client*, 
          FabricCore::RTVal, 
          int , 
          QGLContext*, 
          QWidget *parent = 0, 
          QGLWidget *shared = 0,
          QSettings *settings = 0);
        
        virtual ~RTRGLViewportWidget();

        void setTime(float time);
        void toggleAlwaysRefresh();
        bool alwaysRefreshes() { return m_alwaysRefresh; }
 
        virtual FabricCore::RTVal getCamera();
        virtual void setBackgroundColor(QColor color) {};


      signals:
        void sceneChanged();
        void viewportDestroying();
        void manipsAcceptedEvent(bool);
        void synchronizeCommands(bool);


      public slots:
        virtual void onContextMenu(const QPoint &point);
        void editObjectColor();
        void editLocalObjectColor();

        void addLight();
        void addArchive();
        void addTexture();
        void editLightProperties();


      private:
        void constuctAddMenu();
        void constuctLightMenu();
        bool onEvent(QEvent *event);
        void editObjectColor( bool local );
        void addExternalFile(QStringList, QPoint, bool);
        void constuctGeometryMenu(std::string category);


      protected:
        virtual void paintGL();
        virtual void resizeGL(int w, int h);

        virtual void wheelEvent(QWheelEvent *event);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseReleaseEvent(QMouseEvent *event);

        virtual void dropEvent(QDropEvent *event);
        virtual void dragMoveEvent(QDragMoveEvent *event);
        virtual void dragEnterEvent(QDragEnterEvent *event);


        QWidget *m_parent;
        QPoint m_screenPos;
        int m_viewportIndex;
        bool m_alwaysRefresh;

        FabricCore::RTVal m_width;
        FabricCore::RTVal m_height;
        FabricCore::RTVal m_shObject;
        FabricCore::RTVal m_viewportIndexRTVal;

        FabricUI::SceneHub::SGLightManagerDialog *m_lightDialog;
        FabricUI::SceneHub::SGBaseManagerDialog *m_geometryDialog;
    };
  };
};

#endif // __FABRICUI_SCENEHUB_RTRGLVIEWPORTWIDGET_H__
