// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QApplication>
#include <QtGui/QGraphicsSceneHoverEvent>

#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <FabricUI/GraphView/Graph.h>

using namespace FabricUI::GraphView;

BackDropNode::BackDropNode(
  Graph * parent,
  FTL::CStrRef name,
  FTL::CStrRef title,
  QColor color,
  QColor titleColor
  )
  : Node( parent, name, title, color, titleColor, true )
{
  m_mainWidget->setMinimumWidth(graph()->config().nodeMinWidth * 2.0f);
  m_mainWidget->setMinimumHeight(graph()->config().nodeMinHeight * 2.0f);
  m_mainWidget->setMaximumWidth(graph()->config().nodeMinWidth * 2.0f);
  m_mainWidget->setMaximumHeight(graph()->config().nodeMinHeight * 2.0f);

  m_resizeDistance = 16.0;
  m_hasCustomPointer = false;
  setAcceptHoverEvents(true);
  setZValue(-100);
}

BackDropNode::~BackDropNode()
{
}

void BackDropNode::setSize( QSizeF size )
{
  m_mainWidget->setMinimumSize( size );
  m_mainWidget->setMaximumSize( size );
}

void BackDropNode::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  if(m_hasCustomPointer)
  {
    QApplication::restoreOverrideCursor();
    m_hasCustomPointer = false;
  }
  Node::hoverLeaveEvent(event);
}

void BackDropNode::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  if(m_dragging == 0)
  {
    int corner = getCorner(event->pos());

    if(!m_hasCustomPointer && (corner == 0 || corner == 3))
    {
      QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
      m_hasCustomPointer = true;
    }
    else if(!m_hasCustomPointer && (corner == 1 || corner == 2))
    {
      QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
      m_hasCustomPointer = true;
    }
    else if(m_hasCustomPointer)
    {
      QApplication::restoreOverrideCursor();
      m_hasCustomPointer = false;
    }
  }

  Node::hoverMoveEvent(event);
}

void BackDropNode::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  int corner = getCorner(event->pos());
  if(corner != -1)
  {
    m_dragging = 3 + corner;
    m_dragButton = event->button();
    event->accept();
    return;
  }

  Node::mousePressEvent(event);
}

void BackDropNode::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  QPointF delta = event->scenePos() - event->lastScenePos();
  delta *= 1.0f / graph()->mainPanel()->canvasZoom();

  if(m_dragging == 3) // topleft
  {
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + delta,
      m_mainWidget->minimumSize() + QSizeF( -delta.x(), -delta.y() ),
      false
      );
    event->accept();
    return;
  }
  
  if(m_dragging == 4) // topright
  {
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + QPointF( 0, delta.y() ),
      m_mainWidget->minimumSize() + QSizeF( +delta.x(), -delta.y() ),
      false
      );
    event->accept();
    return;
  }
  
  if(m_dragging == 5) // bottomleft
  {
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + QPointF( delta.x(), 0 ),
      m_mainWidget->minimumSize() + QSizeF( -delta.x(), +delta.y() ),
      false
      );
    event->accept();
    return;
  }
  
  if(m_dragging == 6) // bottomright
  {
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + QPointF( 0, 0 ),
      m_mainWidget->minimumSize() + QSizeF( +delta.x(), +delta.y() ),
      false
      );
    event->accept();
    return;
  }

  Node::mouseMoveEvent(event);
}

void BackDropNode::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  QPointF preDelta =
    event->buttonDownScenePos( m_dragButton ) - event->lastScenePos();
  QPointF delta =
    event->scenePos() - event->buttonDownScenePos( m_dragButton );

  if(m_dragging == 3) // topleft
  {
    m_dragging = 0;
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + preDelta,
      m_mainWidget->minimumSize() + QSizeF( -preDelta.x(), -preDelta.y() ),
      false
      );
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + delta,
      m_mainWidget->minimumSize() + QSizeF( -delta.x(), -delta.y() ),
      true
      );
    event->accept();
    return;
  }
  
  if(m_dragging == 4) // topright
  {
    m_dragging = 0;
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + QPointF( 0, preDelta.y() ),
      m_mainWidget->minimumSize() + QSizeF( +preDelta.x(), -preDelta.y() ),
      false
      );
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + QPointF( 0, delta.y() ),
      m_mainWidget->minimumSize() + QSizeF( +delta.x(), -delta.y() ),
      true
      );
    event->accept();
    return;
  }
  
  if(m_dragging == 5) // bottomleft
  {
    m_dragging = 0;
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + QPointF( preDelta.x(), 0 ),
      m_mainWidget->minimumSize() + QSizeF( -preDelta.x(), +preDelta.y() ),
      false
      );
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + QPointF( delta.x(), 0 ),
      m_mainWidget->minimumSize() + QSizeF( -delta.x(), +delta.y() ),
      true
      );
    event->accept();
    return;
  }
  
  if(m_dragging == 6) // bottomright
  {
    m_dragging = 0;
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + QPointF( 0, 0 ),
      m_mainWidget->minimumSize() + QSizeF( +preDelta.x(), +preDelta.y() ),
      false
      );
    graph()->controller()->gvcDoResizeBackDropNode(
      this,
      topLeftGraphPos() + QPointF( 0, 0 ),
      m_mainWidget->minimumSize() + QSizeF( +delta.x(), +delta.y() ),
      true
      );
    event->accept();
    return;
  }

  Node::mouseReleaseEvent(event);
}

void BackDropNode::appendOverlappingNodes( std::vector<Node*> &nodes ) const
{
  std::vector<Node *> all = graph()->nodes();

  QPointF topLeft = mapToScene(boundingRect().topLeft());
  QPointF bottomRight = mapToScene(boundingRect().bottomRight());
  QRectF rect(topLeft, bottomRight);

  for(size_t i=0;i<all.size();i++)
  {
    if ( all[i]->isBackDropNode() )
      continue;
    if(all[i]->selected())
      continue;

    QPointF topLeft2 = all[i]->mapToScene(all[i]->boundingRect().topLeft());
    QPointF bottomRight2 = all[i]->mapToScene(all[i]->boundingRect().bottomRight());
    QRectF rect2(topLeft2, bottomRight2);

    if(rect.contains(rect2))
      nodes.push_back(all[i]);
  }
}

int BackDropNode::getCorner(QPointF pos)
{
  QPointF topLeft = mapFromItem(m_mainWidget, m_mainWidget->boundingRect().topLeft());
  QPointF bottomRight = mapFromItem(m_mainWidget, m_mainWidget->boundingRect().bottomRight());
  QRectF rect(topLeft, bottomRight);

  if((rect.topLeft() - pos).manhattanLength() < m_resizeDistance)
    return 0;
  else if((rect.topRight() - pos).manhattanLength() < m_resizeDistance)
    return 1;
  else if((rect.bottomLeft() - pos).manhattanLength() < m_resizeDistance)
    return 2;
  else if((rect.bottomRight() - pos).manhattanLength() < m_resizeDistance)
    return 3;

  return -1;
}
