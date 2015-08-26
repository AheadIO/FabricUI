// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_MouseGrabber__
#define __UI_GraphView_MouseGrabber__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QToolTip>
#include "ConnectionTarget.h"
#include "PortType.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Node;
    class Graph;
    class ConnectionTarget;
    class Connection;

    class MouseGrabber : public ConnectionTarget
    {
      Q_OBJECT

    public:

      MouseGrabber(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType);

      float radius() const;
      float diameter() const;
      ConnectionTarget * target();
      const ConnectionTarget * target() const;
      Graph * graph();
      const Graph * graph() const;
      virtual QColor color() const;

      virtual std::string path() const { return "***ASSERT***"; }

      virtual bool canConnectTo(
        ConnectionTarget * other,
        std::string &failureReason
        ) const
        { return false; }

      virtual TargetType targetType() const { return TargetType_MouseGrabber; }
      virtual QPointF connectionPos(PortType pType) const;

      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

      // these don't do anything in this case
      virtual bool highlighted() const { return false; }
      virtual void setHighlighted(bool state = true) {}

    signals:

      void positionChanged(QPointF);

    private slots:

      void nodeHeaderMenuTriggered(QAction *);

    private:

      void showToolTip();
      void invokeConnect(ConnectionTarget * source, ConnectionTarget * target);
      void invokeNodeHeaderMenu(Node * node, ConnectionTarget * other, PortType nodeRole, QPoint pos);

      QPointF m_connectionPos;
      ConnectionTarget * m_target;
      PortType m_otherPortType;
      float m_radius;
      Connection * m_connection;
      ConnectionTarget * m_targetUnderMouse;

      Node * m_contextNode;
      ConnectionTarget * m_contextOther;
      PortType m_contextNodeRole;
    };

  };

};

#endif // __UI_GraphView_MouseGrabber__
