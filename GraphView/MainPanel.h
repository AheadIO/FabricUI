// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_MainPanel__
#define __UI_GraphView_MainPanel__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QPen>
#include <QtGui/QColor>
#include <vector>

#include "SelectionRect.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;
    class Node;

    class MainPanel : public QGraphicsWidget
    {
      Q_OBJECT

    public:

      enum ManipulationMode
      {
        ManipulationMode_None,
        ManipulationMode_Select,
        ManipulationMode_Pan,
        ManipulationMode_Zoom
      };

      MainPanel(Graph * parent);
      virtual ~MainPanel() {}

      Graph * graph();
      const Graph * graph() const;
      QGraphicsWidget * itemGroup();
      const QGraphicsWidget * itemGroup() const;

      float canvasZoom() const;
      QPointF canvasPan() const;
 
      float mouseWheelZoomRate() const;
      void setMouseWheelZoomRate(float rate);
      ManipulationMode manipulationMode() const;

      virtual QRectF boundingRect() const;
      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
      virtual void wheelEvent(QGraphicsSceneWheelEvent * event);
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
      virtual void resizeEvent(QGraphicsSceneResizeEvent * event);

      // used by controller
      void setCanvasZoom(float state, bool quiet = false);
      void setCanvasPan(QPointF pos, bool quiet = false);
      void setAlwaysPan(bool state) {m_alwaysPan = state;}
      bool grabsEvent( QEvent * e ); // When manipulating camera, events must be forwarded here and not to individual widgets (Alt, Space)

    signals:

      void canvasZoomChanged(float zoom);
      void canvasPanChanged(QPointF pos);
      void geometryChanged();
      
    protected:

      void performZoom(
        float zoomFactor,
        QPointF zoomCenter
        );

    private:

      static const float s_minZoom;
      static const float s_maxZoom;
      static const float s_minZoomForOne;
      static const float s_maxZoomForOne;

      Graph * m_graph;
      float m_mouseWheelZoomRate;
      float m_mouseAltZoomState;
      float m_mouseWheelZoomState;
      ManipulationMode m_manipulationMode;
      QGraphicsWidget * m_itemGroup;
      bool m_draggingSelRect;
      QPointF m_lastPanPoint;
      SelectionRect * m_selectionRect;
      bool m_alwaysPan;
      std::vector<Node*> m_ongoingSelection;
      QRectF m_boundingRect;
    };

  };

};

#endif // __UI_GraphView_MainPanel__
