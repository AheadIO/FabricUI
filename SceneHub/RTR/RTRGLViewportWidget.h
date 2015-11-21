#ifndef __FABRICUI_VIEWPORTS_RTRGLVIEWPORTWIDGET__
#define __FABRICUI_VIEWPORTS_RTRGLVIEWPORTWIDGET__

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
#include <FabricUI/SceneHub/Managers/SGLightManagerDialog.h>
#include <FabricUI/SceneHub/Managers/SGGeometryManagerDialog.h>
#include <FabricUI/SceneHub/Commands/SHCmdView.h>

namespace FabricUI
{
  namespace Viewports
  {
    class RTRGLViewportWidget : public QGLWidget
    {
      Q_OBJECT

      public:
        RTRGLViewportWidget(
          FabricCore::Client *, 
          FabricCore::RTVal, 
          int, 
          QGLContext *, 
          QWidget *parent = NULL, 
          QGLWidget *shared = NULL);
        
        virtual ~RTRGLViewportWidget();

        void initialize();
        void resetRTVals();
        void setTime(float time);
        void toggleAlwaysRefresh();
        bool alwaysRefreshes() { return m_alwaysRefresh; }
        double fps() const { return m_fps; }

      signals:
        void sceneChanged();
        void manipsAcceptedEvent( bool );
        void viewportDestroying();
        void updateCommands();

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
        bool onEvent(QEvent *event);

      protected:
        virtual void paintGL();
        virtual void resizeGL(int w, int h);

        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);
        virtual void mouseReleaseEvent(QMouseEvent *event);
        virtual void wheelEvent(QWheelEvent *event);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void dragEnterEvent(QDragEnterEvent *event);
        virtual void dragMoveEvent(QDragMoveEvent *event);
        virtual void dropEvent(QDropEvent *event);

        QWidget *m_parent;
        FabricCore::Client *m_client;

        FabricCore::RTVal m_shObject;
        int m_viewportIndex;

        FabricCore::RTVal m_viewport;
        FabricCore::RTVal m_viewportIndexRTVal;
        FabricCore::RTVal m_width;
        FabricCore::RTVal m_height;

        QPoint m_screenPos;
        QTime m_fpsTimer;
        double m_fps;
        double m_fpsStack[16];

        FabricUI::SceneHub::SGBaseManagerDialog *m_geometryDialog;
        FabricUI::SceneHub::SGLightManagerDialog *m_lightDialog;
        bool m_alwaysRefresh;
    };
  };
};

#endif // __FABRICUI_VIEWPORTS_RTRGLVIEWPORTWIDGET__
