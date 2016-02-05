// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QGraphicsSceneHoverEvent>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>

#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Port.h>

#include <algorithm>
#include <cstdlib>

using namespace FabricUI::GraphView;

Connection::Connection(
  Graph * graph,
  ConnectionTarget * src,
  ConnectionTarget * dst,
  bool forceUseOfPinColor
  )
  : QObject( graph->itemGroup() )
  , QGraphicsPathItem( graph->itemGroup() )
  , m_graph( graph )
  , m_src( src )
  , m_dst( dst )
  , m_hovered( false )
  , m_dragging( false )
  , m_aboutToBeDeleted( false )
  , m_hasSelectedTarget( false )
{
  m_isExposedConnection = 
    m_src->targetType() == TargetType_ProxyPort ||
    m_src->targetType() == TargetType_Port ||
    m_dst->targetType() == TargetType_ProxyPort ||
    m_dst->targetType() == TargetType_Port;

  if(m_isExposedConnection)
  {
    m_defaultPen = m_graph->config().connectionExposePen;
  }
  else
  {
    m_defaultPen = m_graph->config().connectionDefaultPen;
  }
  m_hoverPen = m_graph->config().connectionHoverPen;

  m_clipRadius = m_graph->config().connectionExposeRadius;

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

  dependencyMoved();
  dependencySelected();

  MainPanel *mainPanel = graph->mainPanel();
  QObject::connect(
    mainPanel, SIGNAL(geometryChanged()),
    this, SLOT(dependencyMoved())
    );
  QObject::connect(
    mainPanel, SIGNAL(canvasZoomChanged(float)),
    this, SLOT(dependencyMoved())
    );
  QObject::connect(
    mainPanel, SIGNAL(canvasPanChanged(QPointF)),
    this, SLOT(dependencyMoved())
    );

  for(int i=0;i<2;i++)
  {
    ConnectionTarget * target = src;
    if(i>0)
      target = dst;

    if(target->targetType() == TargetType_Pin)
    {
      Pin * pin = (Pin*)target;
      Node * node = pin->node();
      QObject::connect(pin, SIGNAL(visibleChanged()), this, SLOT(dependencyMoved()), Qt::QueuedConnection);
      QObject::connect(pin, SIGNAL(yChanged()), this, SLOT(dependencyMoved()), Qt::QueuedConnection);
      QObject::connect(node, SIGNAL(positionChanged(FabricUI::GraphView::Node *, QPointF)), this, SLOT(dependencyMoved()));
      QObject::connect(node, SIGNAL(geometryChanged()), this, SLOT(dependencyMoved()));
      QObject::connect(node, SIGNAL(selectionChanged(FabricUI::GraphView::Node *, bool)), this, SLOT(dependencySelected()));
    }
    else if(target->targetType() == TargetType_MouseGrabber)
    {
      MouseGrabber * grabber = (MouseGrabber*)target;
      QObject::connect(grabber, SIGNAL(positionChanged(QPointF)), this, SLOT(dependencyMoved()));
    }
    else if(target->targetType() == TargetType_Port)
    {
      Port * port = (Port*)target;
      QObject::connect(port, SIGNAL(positionChanged()), this, SLOT(dependencyMoved()));
      QObject::connect(port->sidePanel(), SIGNAL(scrolled()), this, SLOT(dependencyMoved()));
    }
  }
}

Connection::~Connection()
{
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

void Connection::setColor(QColor color)
{
  if(m_graph->config().connectionUsePinColor)
  {
    QColor defaultColor = m_graph->config().connectionColor;
    float ratio = m_graph->config().connectionNodePortColorRatio;
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

QPointF Connection::srcPoint() const
{
  if(m_aboutToBeDeleted)
    return boundingRect().topLeft();

  return mapFromScene(m_src->connectionPos(PortType_Output));
}

QPointF Connection::dstPoint() const
{
  if(m_aboutToBeDeleted)
    return boundingRect().bottomRight();

  return mapFromScene(m_dst->connectionPos(PortType_Input));
}

void Connection::invalidate()
{
  m_aboutToBeDeleted = true;
  prepareGeometryChange();
}

void Connection::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  m_hovered = true;
  setPen(m_hoverPen);
  QGraphicsPathItem::hoverEnterEvent(event);
}

void Connection::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  m_hovered = false;
  setPen(m_defaultPen);
  QGraphicsPathItem::hoverLeaveEvent(event);
}

void Connection::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(!graph()->isEditable())
    return QGraphicsPathItem::mousePressEvent(event);

  if(event->modifiers().testFlag(Qt::AltModifier))
    return QGraphicsPathItem::mousePressEvent(event);

  if(event->button() == Qt::LeftButton)
  {
    m_dragging = true;
    m_lastDragPoint = mapToScene(event->pos());
    event->accept();
  }
  else if(event->button() == DFG_QT_MIDDLE_MOUSE)
  {
    if(graph()->config().middleClickDeletesConnections)
    {
      graph()->controller()->gvcDoRemoveConnection(this);
      event->accept();
      return;
    }
    else
      QGraphicsPathItem::mousePressEvent(event);
  }
  else if(event->button() == Qt::RightButton)
  {
    QMenu * menu = graph()->getConnectionContextMenu(this);
    if(menu)
    {
      menu->exec(QCursor::pos());
      menu->deleteLater();
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

      if(graph->controller()->gvcDoRemoveConnection(this))
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
  if(m_isExposedConnection && !m_hovered && !m_hasSelectedTarget && m_graph->config().dimConnectionLines)
  {
    painter->setOpacity(0.15);
    QGraphicsPathItem::paint(painter, option, widget);
    painter->setOpacity(0.5);
    painter->setClipPath(m_clipPath);
    QGraphicsPathItem::paint(painter, option, widget);
    painter->setClipping(false);
    painter->setOpacity(1.0);
  }
  else
  {
    QGraphicsPathItem::paint(painter, option, widget);
  }
}

void Connection::dependencyMoved()
{
  QPointF currSrcPoint = srcPoint();
  QPointF currDstPoint = dstPoint();
  float tangentLength = computeTangentLength();

  // painter->setRenderHint(QPainter::Antialiasing,true);
  // painter->setRenderHint(QPainter::HighQualityAntialiasing,true);

  QPainterPath path;
  path.moveTo(currSrcPoint);

  path.cubicTo(
      currSrcPoint + QPointF(tangentLength, 0), 
      currDstPoint - QPointF(tangentLength, 0), 
      currDstPoint
  );

  // we draw the curve the other way as well to
  // ensure that the polygon is closed in the top left.
  // not doing this results in an open polygon, which 
  // make the hover area for the curve very big.
  path.cubicTo(
      currDstPoint - QPointF(tangentLength, 0), 
      currSrcPoint + QPointF(tangentLength, 0), 
      currSrcPoint
  );

  setPath(path);

  if(m_isExposedConnection)
  {
    m_clipPath = QPainterPath();
    m_clipPath.addEllipse(currSrcPoint, m_clipRadius, m_clipRadius);
    m_clipPath.addEllipse(currDstPoint, m_clipRadius, m_clipRadius);
  }
}

void Connection::dependencySelected()
{
  m_hasSelectedTarget = false;

  if(m_src->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)m_src)->node();
    m_hasSelectedTarget = m_hasSelectedTarget || node->selected();
  }
  if(m_dst->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)m_dst)->node();
    m_hasSelectedTarget = m_hasSelectedTarget || node->selected();
  }
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
