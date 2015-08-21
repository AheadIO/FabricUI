// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

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
      void setGraph(Graph * graph);

      virtual void resizeEvent(QResizeEvent * event);
      virtual void dragMoveEvent(QDragMoveEvent *event);
      virtual void dragEnterEvent(QDragEnterEvent *event);
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
      QGraphicsScene * m_scene;
      Graph * m_graph;
    };

  };

};

#endif // __UI_GraphView_GraphViewWidget__
