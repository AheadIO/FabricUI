// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ConnectionTarget.h"
#include "Graph.h"
#include "PinCircle.h"

#include <QtGui/QGraphicsSceneHoverEvent>
#include <QtGui/QGraphicsSceneMouseEvent>

#include <math.h>

using namespace FabricUI::GraphView;

ConnectionTarget::ConnectionTarget(QGraphicsWidget * parent)
  : QGraphicsWidget(parent) 
  , m_lastPinCircle( 0 )
#if defined(FTL_BUILD_DEBUG)
  , m_deleted( false )
#endif
{
  setAcceptHoverEvents(true);
}

ConnectionTarget::~ConnectionTarget()
{
#if defined(FTL_BUILD_DEBUG)
  m_deleted = true;
#endif
}

bool ConnectionTarget::isConnected() const
{
  return graph()->isConnected(this);
}

void ConnectionTarget::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  PinCircle * circle = findPinCircle(event->pos());
  if(circle)
  {
    setHighlighted(true);
    return circle->hoverEnterEvent(event);
  }

  QGraphicsWidget::hoverEnterEvent(event);
}

void ConnectionTarget::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  PinCircle * circle = findPinCircle(event->pos());
  if(circle)
  {
    setHighlighted(false);
    circle->hoverLeaveEvent(event);
    m_lastPinCircle = NULL;
    return;
  }
  QGraphicsWidget::hoverLeaveEvent(event);
}

void ConnectionTarget::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  PinCircle * circle = findPinCircle(event->pos());
  if(circle)
  {
    float pinClickableDistance = graph()->config().pinClickableDistance;
    QPointF center = circle->centerInSceneCoords();
    QPointF clicked = mapToScene(event->pos());
    float x = center.x() - clicked.x();
    float y = center.y() - clicked.y();
    float distance = sqrt(x * x + y * y);
    if(distance < pinClickableDistance)
    {
      circle->mousePressEvent(event);
      m_lastPinCircle = NULL;
      return;
    }
  }

  QGraphicsWidget::mousePressEvent(event);
}

PinCircle * ConnectionTarget::findPinCircle(QPointF pos)
{
  if(m_lastPinCircle == NULL)
  {
    if(type() == QGraphicsItemType_ProxyPort)
    {
      ProxyPort * target = static_cast<ProxyPort *>(this);
      m_lastPinCircle = target->circle();
    }
    else if(type() == QGraphicsItemType_Port)
    {
      Port * target = static_cast<Port *>(this);
      m_lastPinCircle = target->circle();
    }
    else if(type() == QGraphicsItemType_Pin)
    {
      Pin * target = static_cast<Pin *>(this);
      if(pos.x() < size().width() * 0.5)
      {
        if(target->inCircle())
          m_lastPinCircle = target->inCircle();
      }
      else
      {
        if(target->outCircle())
          m_lastPinCircle = target->outCircle();
      }
    }
  }
  return m_lastPinCircle;
}
