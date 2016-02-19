// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "NodeBubble.h"
#include "Graph.h"
#include "Controller.h"

#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsSimpleTextItem>
#include <QtGui/QPainter>

namespace FabricUI {
namespace GraphView {

QSizeF const NodeBubble::s_topLeftTextMargins( 4.0, 4.0 );
QSizeF const NodeBubble::s_bottomRightTextMargins( 4.0, 4.0 );

NodeBubble::NodeBubble(
  Node *node,
  GraphConfig const &config
  )
  : QGraphicsObject( node->m_graph->itemGroup() )
  , m_node( node )
  , m_nodeCornerRadius( config.nodeCornerRadius )
  , m_nodeWidthReduction( config.nodeWidthReduction )
  , m_minSize( config.nodeBubbleMinWidth, config.nodeBubbleMinHeight )
  , m_rectItem( new QGraphicsRectItem( this ) )
  , m_textItem( new QGraphicsSimpleTextItem( this ) )
{
  setZValue( -50 );

  m_rectItem->setPen( m_node->m_defaultPen );
  m_rectItem->setBrush( config.nodeBubbleColor );

  m_textItem->setPos(
    s_topLeftTextMargins.width(),
    s_topLeftTextMargins.height()
    );
  m_textItem->setBrush( m_node->m_defaultPen.color() );
  m_textItem->setFont( config.nodeBubbleFont );
  
  // [pzion 20160218] This should not be necessary; however, there seems
  // to be a bug in OpenGL rendering of QGraphicsView that causes this
  // text to not render properly without this cache...
  m_textItem->setCacheMode( DeviceCoordinateCache );

  m_node->m_bubble = this;

  QObject::connect(
    m_node, SIGNAL(positionChanged(FabricUI::GraphView::Node *, QPointF)),
    this, SLOT(onNodePositionChanged(FabricUI::GraphView::Node *, QPointF))
    );
  QObject::connect(
    this, SIGNAL(bubbleEditRequested(FabricUI::GraphView::NodeBubble *)), 
    m_node, SLOT(onBubbleEditRequested(FabricUI::GraphView::NodeBubble *))
    );

  updateChildrenGeometries();
}

NodeBubble::~NodeBubble()
{
}

QRectF NodeBubble::boundingRect() const
{
  return m_rectItem->boundingRect();
}

bool NodeBubble::isCollapsed() const
{
  return !m_textItem->isVisible();
}

bool NodeBubble::isExpanded() const
{
  return !isCollapsed();
}

void NodeBubble::expand()
{
  if ( isCollapsed() )
  {
    m_textItem->show();

    updateChildrenGeometries();
  }
}

void NodeBubble::collapse()
{
  if ( isExpanded() )
  {
    m_textItem->hide();

    updateChildrenGeometries();
  }
}

QString NodeBubble::text() const
{
  return m_textItem->text();
}

void NodeBubble::setText( QString t )
{
  m_textItem->setText( t );

  updateChildrenGeometries();
}

void NodeBubble::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
  if ( event->button() == Qt::RightButton )
  {
    // toggle the expanded state
    m_node->m_graph->controller()->gvcDoSetNodeCommentExpanded(
      m_node,
      isCollapsed()
      );
  }

  event->accept();
}

void NodeBubble::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
  if ( isCollapsed() )
    m_node->m_graph->controller()->gvcDoSetNodeCommentExpanded( m_node, true );
  else
    emit bubbleEditRequested( this );

  event->accept();
}

void NodeBubble::onNodePositionChanged(
  FabricUI::GraphView::Node *node,
  QPointF pos
  )
{
  updateChildrenGeometries();
}

void NodeBubble::updateChildrenGeometries()
{
  QSizeF effectiveTextSize;
  if ( m_textItem->isVisible() )
    effectiveTextSize =
      ( s_topLeftTextMargins
      + m_textItem->boundingRect().size()
      + s_bottomRightTextMargins ).expandedTo( m_minSize );
  else
    effectiveTextSize = m_minSize;

  m_rectItem->setRect( QRectF( QPointF( 0, 0 ), effectiveTextSize ) );

  QPointF p = m_node->topLeftGraphPos();
  p += QPointF(
    m_nodeCornerRadius * 0.5 + m_nodeWidthReduction * 0.5f,
    -effectiveTextSize.height()
    );
  setPos( p );
}

void NodeBubble::paint(
  QPainter *painter,
  QStyleOptionGraphicsItem const *option,
  QWidget *widget
  )
{
}

} // namespace GraphView
} // namespace FabricUI
