// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/MouseGrabber.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Connection.h>

#include <QtGui/QPainter>
#include <QtGui/QCursor>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsView>
#include <QtGui/QMenu>
#include <QtGui/QAction>

using namespace FabricUI::GraphView;

MouseGrabber::MouseGrabber(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType)
: ConnectionTarget(parent->itemGroup())
{
  m_connectionPos = mousePos;
  m_target = target;
  m_otherPortType = portType;
  m_targetUnderMouse = NULL;
  m_contextNode = NULL;
  m_contextOther = NULL;

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
}

MouseGrabber::~MouseGrabber()
{
}

MouseGrabber * MouseGrabber::construct(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType)
{
  if(target->targetType() == TargetType_NodeHeader)
  {
    NodeHeader * header = static_cast<NodeHeader *>( target );
    Node * node = header->node();

    QMenu * menu = createNodeHeaderMenu(node, NULL, portType);
    if(menu == NULL)
      return NULL;

    QPoint globalPos = QCursor::pos();
    QAction * action = menu->exec(globalPos);

    if(action == NULL)
      return NULL;

    QString name = action->data().toString();
    if(name == "")
      return NULL;

    Pin * pin = node->pin(name.toUtf8().constData());
    if(!pin)
      return NULL;

    return construct(parent, mousePos, pin, portType);
  }
  return new MouseGrabber(parent, mousePos, target, portType);
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
              QPoint widgetPos = view->mapFromScene( QPoint(
                int( m_connectionPos.x() ), int( m_connectionPos.y() ) ) );
              QPoint globalPos = view->mapToGlobal(widgetPos);
              
              // map the exit rect. if the mouse leaves the area the tooltip will disappear
              QRectF circleRect = pinCircle->boundingRect();
              QPointF circleTopLeft = circleRect.topLeft();
              QPointF circleBottomRight = circleRect.bottomRight();
              QPointF sceneCircleTopLeft = pinCircle->mapToScene(circleTopLeft);
              QPointF sceneCircleBottomRight = pinCircle->mapToScene(circleBottomRight);
              QPoint widgetCircleTopLeft =
                view->mapFromScene( QPoint( int( sceneCircleTopLeft.x() ),
                                            int( sceneCircleTopLeft.y() ) ) );
              QPoint widgetCircleBottomRight = view->mapFromScene(
                QPoint( int( sceneCircleBottomRight.x() ),
                        int( sceneCircleBottomRight.y() ) ) );

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

    invokeConnect(source, target);
  }

  if(!event->modifiers().testFlag(Qt::ShiftModifier))
  {
    prepareGeometryChange();
    ungrabMouse();
    QGraphicsScene * scene = graph()->scene();
    graph()->resetMouseGrabber();
    m_connection->invalidate();
    scene->removeItem(m_connection);
    m_connection->deleteLater();
    // m_connection->setParent(this);
    scene->removeItem(this);
    scene->update();
    this->deleteLater();
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

void MouseGrabber::invokeConnect(ConnectionTarget * source, ConnectionTarget * target)
{
  if(source->targetType() == TargetType_ProxyPort && target->targetType() == TargetType_Pin)
  {
    Pin *pinToConnectWith = static_cast<Pin *>( target );
    graph()->controller()->gvcDoAddPort(
      pinToConnectWith->name(),
      PortType_Output,
      pinToConnectWith->dataType(),
      pinToConnectWith
      );
  }
  else if(target->targetType() == TargetType_ProxyPort && source->targetType() == TargetType_Pin)
  {
    Pin *pinToConnectWith = static_cast<Pin *>( source );
    graph()->controller()->gvcDoAddPort(
      pinToConnectWith->name(),
      PortType_Input,
      pinToConnectWith->dataType(),
      pinToConnectWith
      );
  }
  else if(source->targetType() == TargetType_NodeHeader)
  {
    NodeHeader * header = static_cast<NodeHeader *>( source );
    Node * node = header->node();

    QPoint globalPos = QCursor::pos();
    invokeNodeHeaderMenu(node, target, PortType_Input, globalPos);
  }
  else if(target->targetType() == TargetType_NodeHeader)
  {
    NodeHeader * header = static_cast<NodeHeader *>( target );
    Node * node = header->node();

    QPoint globalPos = QCursor::pos();
    invokeNodeHeaderMenu(node, source, PortType_Output, globalPos);
  }
  else
  {
    graph()->controller()->gvcDoAddConnection(source, target);
  }
}

QMenu * MouseGrabber::createNodeHeaderMenu(Node * node, ConnectionTarget * other, PortType nodeRole)
{
  QMenu *menu = new QMenu(NULL);

  unsigned int count = 0;
  for(unsigned int i=0;i<node->pinCount();i++)
  {
    Pin * pin = node->pin(i);

    if(pin->portType() == nodeRole)
      continue;

    if(other)
    {
      if(nodeRole == PortType_Output)
      {
        std::string failureReason;
        if(!other->canConnectTo(pin, failureReason))
          continue;
      }
      else if(nodeRole == PortType_Input)
      {
        std::string failureReason;
        if(!pin->canConnectTo(other, failureReason))
          continue;
      }
    }

    QString name = pin->name().c_str();
    QString label;
    if(nodeRole == PortType_Input)
      label = "> "+name;
    else
      label = name+" <";
    QAction * action = new QAction(label, NULL);
    action->setData(name);
    menu->addAction(action);
    count++;
  }

  if(count == 0)
  {
    delete menu;
    menu = NULL;
  }

  return menu;
}

void MouseGrabber::invokeNodeHeaderMenu(Node * node, ConnectionTarget * other, PortType nodeRole, QPoint pos)
{
  m_contextNode = node;
  m_contextNodeRole = nodeRole;
  m_contextOther = other;

  QMenu * menu = createNodeHeaderMenu(node, other, nodeRole);
  if(menu == NULL)
    return;

  QAction * action = menu->exec(pos);
  if(action == NULL)
    return;

  QString name = action->data().toString();
  if(name == "")
    return;

  Pin * pin = m_contextNode->pin(name.toUtf8().constData());
  if(!pin)
    return;

  if(m_contextOther == NULL)
  {
    // this is invoked from MouseGrabber::construct
    m_contextOther = pin;
  }
  else if(m_contextNodeRole == PortType_Output)
    invokeConnect(m_contextOther, pin);
  else
    invokeConnect(pin, m_contextOther);
}
