// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphViewWidget__
#define __UI_GraphView_GraphViewWidget__

#include <QtGui/QGraphicsView>
#include <QtGui/QColor>
#include <QtGui/QPen>

#include "GraphConfig.h"
#include "Graph.h"

namespace FabricUI
{

  namespace GraphView
  {

    // The GraphViewScene specializes QGraphicsScene for intercepting events that need to be treated
    // globally (eg: click for panning the camera) and not forwareded to individual scene widgets.
    // Note: using event filters on scene widgets doesn't work because 'hover' and 'mouse grabber'
    //       features are bypassing the normal event propagation process.
    class GraphViewScene : public QGraphicsScene
    {
      Q_OBJECT

    public:

      GraphViewScene(
        Graph * graph
        );

    protected:

      virtual bool event(QEvent * e);

    private:

      Graph * m_graph;
    };

    class GraphViewWidget : public QGraphicsView
    {
      Q_OBJECT

    public:

      GraphViewWidget(
        QWidget * parent,
        const GraphConfig & config = GraphConfig(),
        Graph * graph = NULL
        );

      Graph * graph();
      const Graph * graph() const;
      virtual void setGraph(Graph * graph);

      virtual void resizeEvent(QResizeEvent * event);
      virtual void dragMoveEvent(QDragMoveEvent *event);
      virtual void dropEvent(QDropEvent *event);
      virtual void mouseMoveEvent(QMouseEvent * event);
      virtual void keyPressEvent(QKeyEvent * event);
      virtual void keyReleaseEvent(QKeyEvent * event);
      QPoint lastEventPos() const;
      
    public slots:

      void onSceneChanged();

    signals:

      void sceneChanged();

    protected:

      virtual bool focusNextPrevChild(bool next);
      
    private:

      QPoint m_lastEventPos;
      GraphViewScene * m_scene;
      Graph * m_graph;
    };

  };

};

#endif // __UI_GraphView_GraphViewWidget__
