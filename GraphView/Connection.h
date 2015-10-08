// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Connection__
#define __UI_GraphView_Connection__

#include <QtGui/QGraphicsPathItem>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include "ConnectionTarget.h"
#include "GraphicItemTypes.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;

    class Connection : public QObject, public QGraphicsPathItem
    {
      Q_OBJECT

    public:

      Connection(
        Graph * parent,
        ConnectionTarget * src,
        ConnectionTarget * dst,
        bool forceUseOfPinColor = false
        );

      ~Connection();

      virtual int type() const { return QGraphicsItemType_Connection; }

      Graph * graph();
      const Graph * graph() const;
      ConnectionTarget * src();
      const ConnectionTarget * src() const;
      ConnectionTarget * dst();
      const ConnectionTarget * dst() const;

      virtual QColor color() const
        { return m_color; }
      virtual void setColor(QColor color);

      virtual QRectF boundingRect() const;

      QPointF srcPoint() const;
      QPointF dstPoint() const;

      virtual void invalidate();
      
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    public slots:

      virtual void dependencyMoved();
      virtual void dependencySelected();

    private:

      float computeTangentLength() const;

      Graph * m_graph;
      ConnectionTarget * m_src;
      ConnectionTarget * m_dst;

      QColor m_color;
      QPen m_defaultPen;
      QPen m_hoverPen;
      bool m_hovered;

      bool m_dragging;
      QPointF m_lastDragPoint;
      bool m_aboutToBeDeleted;
      bool m_isExposedConnection;
      bool m_hasSelectedTarget;
      bool m_hasNeverDrawn;
      float m_clipRadius;
      QPainterPath m_clipPath;
    };

  };

};

#endif // __UI_GraphView_Connection__
