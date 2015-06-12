// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/MouseGrabber.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Connection.h>

#include <QtGui/QPainter>
#include <QtGui/QCursor>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsView>

using namespace FabricUI::GraphView;

MouseGrabber::MouseGrabber(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType)
: ConnectionTarget(parent->itemGroup())
{
  m_connectionPos = mousePos;
  m_target = target;
  m_otherPortType = portType;
  m_targetUnderMouse = NULL;

  const GraphConfig & config = parent->config();
  m_radius = config.mouseGrabberRadius;

  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  setPreferredSize(QSizeF(diameter(), diameter()));
  setPos(-radius(), -radius());
  setWindowFrameMargins(0, 0, 0, 0);

  QPointF localPos = parent->itemGroup()->mapFromScene(m_connectionPos);
  setZValue(-1);
  setTransform(QTransform::fromTranslate(localPos.x(), localPos.y()), false);
  grabMouse();

  if(m_otherPortType == PortType_Input)
    m_connection = new Connection(parent, m_target, this, true /* forceUseOfPinColor */);
  else
    m_connection = new Connection(parent, this, m_target, true /* forceUseOfPinColor */);

  // self.__mouseOverPinCircle = None
}

float MouseGrabber::radius() const
{
  return m_radius;
}

float MouseGrabber::diameter() const
{
  return m_radius * 2.0f;
}

ConnectionTarget * MouseGrabber::target()
{
  return m_target;
}

const ConnectionTarget * MouseGrabber::target() const
{
  return m_target;
}

Graph * MouseGrabber::graph()
{
  return target()->graph();
}

const Graph * MouseGrabber::graph() const
{
  return target()->graph();
}

QColor MouseGrabber::color() const
{
  return m_target->color();
}

QPointF MouseGrabber::connectionPos(PortType pType) const
{
  if(m_targetUnderMouse)
    return m_targetUnderMouse->connectionPos(m_otherPortType);
  return m_connectionPos;
}

void MouseGrabber::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  m_connectionPos = mapToScene(event->pos());
  QPointF mousePos = mapToItem(graph()->itemGroup(), event->pos());
  setTransform(QTransform::fromTranslate(mousePos.x(), mousePos.y()), false);

  emit positionChanged(mousePos);

  QList<QGraphicsItem *> items = collidingItems(Qt::IntersectsItemBoundingRect);

  ConnectionTarget * newTargetUnderMouse = NULL;
  ConnectionTarget * prevTargetUnderMouse = m_targetUnderMouse;
  float distance = 1000000.0f;
  for(int i=0;i<items.count();i++)
  {
    if(items[i]->type() == QGraphicsItemType_PinCircle)
    {
      PinCircle * pinCircle = (PinCircle *)items[i];
      ConnectionTarget * target = pinCircle->target();
      std::string failureReason;
      bool success = false;
      if(pinCircle->portType() == PortType_Input)
        success = m_target->canConnectTo(target, failureReason);
      else
        success = target->canConnectTo(m_target, failureReason);
      if(success)
      {
        float newDistance = (pinCircle->centerInSceneCoords() - mousePos).manhattanLength();
        if(newDistance < distance)
        {
          distance = newDistance;
          newTargetUnderMouse = target;
          QToolTip::hideText();
        }
      }
      else
      {
        if(failureReason.length() > 0)
        {
          QString failureReasonQString = failureReason.c_str();
          if(!QToolTip::isVisible() || QToolTip::text() != failureReasonQString)
          {
            QGraphicsView * view = scene()->views()[0];
            if(view)
            {
              // map the position
              QPoint widgetPos = view->mapFromScene(QPoint(m_connectionPos.x(), m_connectionPos.y()));
              QPoint globalPos = view->mapToGlobal(widgetPos);
              
              // map the exit rect. if the mouse leaves the area the tooltip will disappear
              QRectF circleRect = pinCircle->boundingRect();
              QPointF circleTopLeft = circleRect.topLeft();
              QPointF circleBottomRight = circleRect.bottomRight();
              QPointF sceneCircleTopLeft = pinCircle->mapToScene(circleTopLeft);
              QPointF sceneCircleBottomRight = pinCircle->mapToScene(circleBottomRight);
              QPoint widgetCircleTopLeft = view->mapFromScene(QPoint(sceneCircleTopLeft.x(), sceneCircleTopLeft.y()));
              QPoint widgetCircleBottomRight = view->mapFromScene(QPoint(sceneCircleBottomRight.x(), sceneCircleBottomRight.y()));

              QRect rect(widgetCircleTopLeft, widgetCircleBottomRight);
              if(rect.contains(widgetPos))
                QToolTip::showText(globalPos, failureReasonQString, view, rect);
              else
                QToolTip::hideText();
            }
          }
        }
      }
    }
  }

  if(newTargetUnderMouse == NULL && prevTargetUnderMouse != NULL)
  {
    prevTargetUnderMouse->setHighlighted(false);
    prevTargetUnderMouse = NULL;
  }
  else
  {
    if(prevTargetUnderMouse != NULL && prevTargetUnderMouse != newTargetUnderMouse)
      prevTargetUnderMouse->setHighlighted(false);
    if(newTargetUnderMouse)
      newTargetUnderMouse->setHighlighted(true);
  }

  m_targetUnderMouse = newTargetUnderMouse;
}

void MouseGrabber::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  if(!event->modifiers().testFlag(Qt::ShiftModifier))
  {
    prepareGeometryChange();
    ungrabMouse();
    QGraphicsScene * scene = graph()->scene();
    graph()->resetMouseGrabber();
    scene->removeItem(m_connection);
    scene->removeItem(this);
    scene->update();
    this->deleteLater();
  }

  if(m_targetUnderMouse)
  {
    m_targetUnderMouse->setHighlighted(false);

    ConnectionTarget * source;
    ConnectionTarget * target;

    if(m_otherPortType == PortType_Input)
    {
      source = m_target;
      target = m_targetUnderMouse;
    }
    else
    {
      source = m_targetUnderMouse;
      target = m_target;
    }

    if(source->targetType() == TargetType_ProxyPort && target->targetType() == TargetType_Pin)
    {
      graph()->controller()->addPortFromPin((Pin*)target, PortType_Output);
    }
    else if(target->targetType() == TargetType_ProxyPort && source->targetType() == TargetType_Pin)
    {
      graph()->controller()->addPortFromPin((Pin*)source, PortType_Input);
    }
    else
    {
      std::vector<Connection*> connections = graph()->connections();
      for(size_t i=0;i<connections.size();i++)
      {
        if(connections[i]->dst() == target)
        {
          // filter out IO ports
          if(connections[i]->src()->targetType() == TargetType_Port && connections[i]->dst()->targetType() == TargetType_Port)
          {
            if(((Port*)connections[i]->src())->name() == ((Port*)connections[i]->dst())->name())
              continue;
          }

          if(!graph()->controller()->removeConnection(connections[i]))
          {
            graph()->controller()->endInteraction();
        
            prepareGeometryChange();
            ungrabMouse();
            QGraphicsScene * scene = graph()->scene();
            scene->removeItem(m_connection);
            scene->removeItem(this);
            scene->update();
            return;
          }
          break;
        }
      }
      graph()->controller()->addConnection(source, target);
    }
  }

  if(!event->modifiers().testFlag(Qt::ShiftModifier))
  {
    // begin interaction is issued by connection +/ PinCircle
    graph()->controller()->endInteraction();
  }
}

void MouseGrabber::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  QColor color = m_connection->color();
  color.setAlpha(125);
  painter->setPen(color);
  painter->drawRect(windowFrameRect());
  QGraphicsWidget::paint(painter, option, widget);
}
