// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_PinCircle__
#define __UI_GraphView_PinCircle__

#include "PortType.h"
#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsEllipseItem>
#include <QtGui/QPen>
#include <QtGui/QColor>
#include "GraphicItemTypes.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class ConnectionTarget;

    class PinCircle : public QGraphicsWidget
    {
      Q_OBJECT

    public:

      PinCircle(ConnectionTarget * parent, PortType portType, QColor color, bool interactiveConnectionsAllowed = true);
      virtual ~PinCircle() {}

      virtual int type() const { return QGraphicsItemType_PinCircle; }

      ConnectionTarget * target();
      const ConnectionTarget * target() const;
      virtual QColor color() const;
      virtual void setColor(QColor color);
      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);
      virtual QPen defaultPen() const;
      virtual QPen hoverPen() const;
      virtual float radius() const;
      virtual float diameter() const;
      virtual QPointF centerInSceneCoords() const;

      PortType portType() const;
      bool isInputPortType() const;
      bool isOutputPortType() const;
      virtual void setClipping(bool state);
      virtual void setShouldBeVisible(bool state);

      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

    private:

      ConnectionTarget * m_target;
      PortType m_portType;
      QColor m_color;
      bool m_highlighted;
      QPen m_defaultPen;
      QPen m_hoverPen;
      float m_radius;
      QGraphicsEllipseItem * m_ellipse;
      bool m_interactiveConnectionsAllowed;
      bool m_shouldBeVisible;
    };

  };

};

#endif // __UI_GraphView_PinCircle__
