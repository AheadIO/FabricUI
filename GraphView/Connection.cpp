// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "Connection.h"
#include "Graph.h"
#include "Pin.h"
#include "Port.h"

#include <QtGui/QGraphicsSceneHoverEvent>
#include <QtGui/QGraphicsSceneMouseEvent>

#include <algorithm>
#include <cstdlib>

using namespace FabricUI::GraphView;

Connection::Connection(Graph * parent, ConnectionTarget * src, ConnectionTarget * dst, bool forceUseOfPinColor)
: QObject(parent->itemGroup())
, QGraphicsPathItem(parent->itemGroup())
{
  m_graph = parent;
  m_src = src;
  m_dst = dst;
  m_aboutToBeDeleted = false;

  m_defaultPen = m_graph->config().connectionDefaultPen;
  m_hoverPen = m_graph->config().connectionHoverPen;

  if(src->targetType() == TargetType_Port || dst->targetType() == TargetType_Port)
  {
    m_defaultPen.setStyle(Qt::DotLine);
    m_hoverPen.setStyle(Qt::DotLine);
  }

  QColor color = m_graph->config().connectionColor;
  if(m_graph->config().connectionUsePinColor || forceUseOfPinColor)
  {
    for(int i=0;i<2;i++)
    {
      ConnectionTarget * target = src;
      if(i>0)
        target = dst;

      if(target->targetType() == TargetType_Pin)
      {
        color = ((Pin*)target)->color();
        break;
      }
      else if(target->targetType() == TargetType_Port)
      {
        color = ((Port*)target)->color();
      }
    }
  }
  setColor(color);
  setAcceptHoverEvents(true);
  setPen(m_defaultPen);

  setZValue(-1);

  m_dragging = false;
  m_boundingBox = QRectF(srcPoint().x(), srcPoint().y(), dstPoint().x(), dstPoint().y());

  for(int i=0;i<2;i++)
  {
    ConnectionTarget * target = src;
    if(i>0)
      target = dst;

    if(target->targetType() == TargetType_Pin)
    {
      Node * node = ((Pin*)target)->node();
      QObject::connect(node, SIGNAL(positionChanged(FabricUI::GraphView::Node *, QPointF)), this, SLOT(dependencyMoved()));
    }
    else if(target->targetType() == TargetType_MouseGrabber)
    {
      MouseGrabber * grabber = (MouseGrabber*)target;
      QObject::connect(grabber, SIGNAL(positionChanged(QPointF)), this, SLOT(dependencyMoved()));
    }
  }
}

Graph * Connection::graph()
{
  return m_graph;
}

const Graph * Connection::graph() const
{
  return m_graph;
}

ConnectionTarget * Connection::src()
{
  return m_src;
}

const ConnectionTarget * Connection::src() const
{
  return m_src;
}

ConnectionTarget * Connection::dst()
{
  return m_dst;
}

const ConnectionTarget * Connection::dst() const
{
  return m_dst;
}

QColor Connection::color() const
{
  return m_color;
}

void Connection::setColor(QColor color)
{
  if(m_graph->config().connectionUsePinColor)
  {
    QColor defaultColor = m_graph->config().connectionColor;
    float ratio = m_graph->config().connectionPinColorRatio;
    float iratio = 1.0 - ratio;
    int r = (int)(float(color.red()) * ratio + float(defaultColor.red()) * iratio);
    int g = (int)(float(color.green()) * ratio + float(defaultColor.green()) * iratio);
    int b = (int)(float(color.blue()) * ratio + float(defaultColor.blue()) * iratio);
    color = QColor(r, g, b);
  }
  else
  {
    color = m_graph->config().connectionColor;
  }

  m_color = color;
  m_defaultPen.setColor(color);
  setPen(m_defaultPen);
}

QPen Connection::defaultPen() const
{
  return m_defaultPen;
}

QPen Connection::hoverPen() const
{
  return m_hoverPen;
}

QPointF Connection::srcPoint() const
{
  if(m_aboutToBeDeleted)
    return m_boundingBox.topLeft();

  return mapFromScene(m_src->connectionPos(PortType_Output));
}

QPointF Connection::dstPoint() const
{
  if(m_aboutToBeDeleted)
    return m_boundingBox.bottomRight();

  return mapFromScene(m_dst->connectionPos(PortType_Input));
}

QRectF Connection::boundingRect() const
{
  return m_boundingBox;
}

void Connection::invalidate()
{
  m_aboutToBeDeleted = true;
}

void Connection::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  setPen(m_hoverPen);
  QGraphicsPathItem::hoverEnterEvent(event);
}

void Connection::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  setPen(m_defaultPen);
  QGraphicsPathItem::hoverLeaveEvent(event);
}

void Connection::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(event->modifiers().testFlag(Qt::AltModifier))
    return QGraphicsPathItem::mousePressEvent(event);

  if(event->button() == Qt::LeftButton)
  {
    m_dragging = true;
    m_lastDragPoint = mapToScene(event->pos());
    event->accept();
  }
  else if(event->button() == Qt::RightButton)
  {
    QMenu * menu = graph()->getConnectionContextMenu(this);
    if(menu)
    {
      menu->exec(QCursor::pos());
      menu->deleteLater();
    }
    else if(graph()->config().rightClickDeletesConnections)
    {
      Graph * g = graph();
      g->controller()->beginInteraction();
      g->controller()->removeConnection(this);
      g->controller()->endInteraction();
      event->accept();
      return;
    }
    else
      QGraphicsPathItem::mousePressEvent(event);
  }
  else
    QGraphicsPathItem::mousePressEvent(event);
}

void Connection::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_dragging)
  {
    QPointF scenePos = mapToScene(event->pos());
    QPointF delta = scenePos - m_lastDragPoint;

    // todo: the disconnect threshold maybe should be a graph setting
    if(delta.x() < 0 || delta.x() > 0)
    {
      // create local variables
      // since "this" might be deleted after the removeConnection call
      ConnectionTarget * src = m_src;
      ConnectionTarget * dst = m_dst;
      Graph * graph = m_graph;

      graph->controller()->beginInteraction();

      if(graph->controller()->removeConnection(this))
      {
        // todo: review the features for disconnecting input vs output based on gesture
        if(delta.x() < 0)
        {
          graph->constructMouseGrabber(scenePos, (Pin*)src, PortType_Input);
        }
        else
        {
          graph->constructMouseGrabber(scenePos, (Pin*)dst, PortType_Output);
        }
      }
      else
      {
        graph->controller()->endInteraction();
      }
    }
  }
  else
    QGraphicsPathItem::mouseMoveEvent(event);
}

void Connection::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  QPointF currSrcPoint = srcPoint();
  QPointF currDstPoint = dstPoint();
  float penWidth = pen().width();
  float tangentLength = computeTangentLength();

  prepareGeometryChange();
  m_boundingBox = m_boundingBox.united(QRectF(
    std::min(currSrcPoint.x(), currDstPoint.x()) - penWidth - tangentLength,
    std::min(currSrcPoint.y(), currDstPoint.y()) - penWidth,
    std::abs(int(currDstPoint.x() - currSrcPoint.x())) + penWidth * 2.0 + tangentLength * 2.0,
    std::abs(int(currDstPoint.y() - currSrcPoint.y())) + penWidth * 2.0
  ));

  QPainterPath path;
  path.moveTo(currSrcPoint);

  path.cubicTo(
      currSrcPoint + QPointF(tangentLength, 0), 
      currDstPoint - QPointF(tangentLength, 0), 
      currDstPoint
  );
  setPath(path);

  QGraphicsPathItem::paint(painter, option, widget);
}

void Connection::dependencyMoved()
{
  m_boundingBox = QRectF(srcPoint().x(), srcPoint().y(), dstPoint().x(), dstPoint().y());
  update();
}

float Connection::computeTangentLength() const
{
  QPointF currSrcPoint = srcPoint();
  QPointF currDstPoint = dstPoint();

  // if the connection points are on the wrong
  // sides, we need to scale up the boundingRect
  float tangentLength = 0.0f;
  if(currSrcPoint.x() < currDstPoint.x())
  {
    tangentLength = m_graph->config().connectionFixedTangentLength;
    tangentLength += m_graph->config().connectionPercentualTangentLength * 0.01 * (currDstPoint.x() - currSrcPoint.x());
  }
  return tangentLength;
}
