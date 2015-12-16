/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_GLVIEWPORT_H__
#define __FABRICUI_GLVIEWPORT_H__


#include <FabricUI/Viewports/ViewportWidget.h>


namespace FabricUI
{
  namespace Viewports
  {
 
    class GLViewportWidget : public ViewportWidget
    {
    	Q_OBJECT

      friend class MainWindow;
      friend class ManipulationTool;

    public:

    	GLViewportWidget(FabricCore::Client * client, QColor bgColor, QGLFormat format, QWidget *parent = NULL, QSettings *settings = NULL);
    	virtual ~GLViewportWidget();
      
      virtual void setBackgroundColor(QColor color);
      virtual bool isManipulationActive() const;
      virtual void setManipulationActive(bool state);
      virtual void clearInlineDrawing();

      FabricCore::RTVal getDrawContext() const { return m_drawContext; }
      virtual ManipulationTool * getManipTool() { return m_manipTool; }

      virtual bool isUsingStage();
      virtual bool isStageVisible();


    public slots:
      virtual void redraw();
      virtual void onKeyPressed(QKeyEvent * event);
      void toggleManipulation() { setManipulationActive(!isManipulationActive()); }
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

      virtual void mousePressEvent(QMouseEvent *event);
      virtual void mouseMoveEvent(QMouseEvent *event);
      virtual void mouseReleaseEvent(QMouseEvent *event);
      virtual void wheelEvent(QWheelEvent *event);

      int m_width;
      int m_height;
      bool m_resizedOnce;
      bool m_stageVisible;
      bool m_usingStage;
      FabricCore::RTVal m_drawing;
      FabricCore::RTVal m_drawContext;

      ManipulationTool * m_manipTool;
    };
  };
};

#include "ManipulationTool.h"

#endif // __FABRICUI_GLVIEWPORT_H__
