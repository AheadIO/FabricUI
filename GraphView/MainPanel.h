// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_MainPanel__
#define __UI_GraphView_MainPanel__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QPen>
#include <QtGui/QColor>

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

      // used by controller
      void setCanvasZoom(float state, bool quiet = false);
      void setCanvasPan(QPointF pos, bool quiet = false);
      void setSpaceBarDown(bool state) {m_spaceBarDown = state;}

    signals:

      void canvasZoomChanged(float zoom);
      void canvasPanChanged(QPointF pos);

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
      bool m_spaceBarDown;
      std::vector<Node*> m_ongoingSelection;
      QRectF m_boundingRect;
    };

  };

};

#endif // __UI_GraphView_MainPanel__
