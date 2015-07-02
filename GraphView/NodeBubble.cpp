// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NodeBubble.h"
#include "Graph.h"
#include "Controller.h"

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QGraphicsDropShadowEffect>
#include <QtCore/QDebug>

using namespace FabricUI::GraphView;

NodeBubble::NodeBubble(Graph * parent, Node * node, const GraphConfig & config)
: QGraphicsWidget(parent->itemGroup())
, m_graph(parent)
, m_node(node)
, m_config(config)
{
  m_collapsed = true;
  m_metrics = new QFontMetrics(m_config.nodeBubbleFont);
  setZValue(-50);

  // connect the bubble to the node
  QObject::connect(m_node, SIGNAL(positionChanged(FabricUI::GraphView::Node *, QPointF)),
    this, SLOT(onNodePositionChanged(FabricUI::GraphView::Node *, QPointF)));
  QObject::connect(this, SIGNAL(bubbleEditRequested(FabricUI::GraphView::NodeBubble *)), 
    m_node, SLOT(onBubbleEditRequested(FabricUI::GraphView::NodeBubble *)));

  m_node->setBubble(this);

  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  updateSize();
}

NodeBubble::~NodeBubble()
{
  delete(m_metrics);
  if(m_node)
  {
    m_node->setBubble(NULL);
  }
}

Graph * NodeBubble::graph()
{
  return m_graph;
}

const Graph * NodeBubble::graph() const
{
  return m_graph;
}

Node * NodeBubble::node()
{
  return m_node;
}

const Node * NodeBubble::node() const
{
  return m_node;
}

void NodeBubble::setNode(Node * node)
{
  m_node = node;  
}

void NodeBubble::expand()
{
  if(!m_collapsed)
    return;
  m_collapsed = false;
  updateSize();
  emit bubbleExpanded(this);
  update();
}

void NodeBubble::collapse()
{
  if(m_collapsed)
    return;
  m_collapsed = true;
  updateSize();
  emit bubbleCollapsed(this);
  update();
}

QString NodeBubble::text() const
{
  return m_text;
}

void NodeBubble::setText(QString t)
{
  if(t == m_text)
    return;
  m_text = t;
  m_textLines.clear();
  if(m_text.length() > 0)
    m_textLines = m_text.split('\n');

  updateSize();
  emit bubbleTextChanged(this, m_text);
  if(!m_collapsed)
    update();
}

void NodeBubble::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(event->button() == Qt::RightButton)
  {
    // toggle the expanded state
    graph()->controller()->setNodeCommentExpanded(m_node, m_collapsed);
    event->accept();
  }
}

void NodeBubble::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_collapsed)
  {
    graph()->controller()->setNodeCommentExpanded(m_node, true);
    event->accept();
  }
  else
  {
    // request editing of a comment!
    emit bubbleEditRequested(this);
  }
}

void NodeBubble::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  painter->setPen(m_node->m_defaultPen);
  painter->setBrush(m_config.nodeBubbleColor);

  QRectF rect = windowFrameRect();
  painter->drawRect(rect);

  if(!m_collapsed && m_textLines.length() > 0)
  {
    painter->setFont(m_config.nodeBubbleFont);

    float offset = 0.0f;
    for(int i=0;i<m_textLines.length();i++)
    {
      offset += m_metrics->lineSpacing();
      painter->drawText(QPointF(4.0f, 4.0f + offset), m_textLines[i]);
    }
  }

  QGraphicsWidget::paint(painter, option, widget);
}

void NodeBubble::onNodePositionChanged(FabricUI::GraphView::Node * node, QPointF pos)
{
  QPointF p = node->topLeftGraphPos();
  p += QPointF(m_config.nodeCornerRadius * 0.5 + m_config.nodeWidthReduction * 0.5f, -size().height());

  setTransform(QTransform::fromTranslate(p.x(), p.y()), false);
}

void NodeBubble::requestEdit()
{
  emit bubbleEditRequested(this);
}

void NodeBubble::updateSize()
{
  float width = 0.0;
  float height = 0.0;

  if(m_collapsed)
  {
    width = m_config.nodeBubbleMinWidth;
    height = m_config.nodeBubbleMinHeight;
  }
  else
  {
    width = m_config.nodeBubbleMinWidth;
    height = m_config.nodeToolbarHeight + 2.0;
    height += float(m_metrics->lineSpacing());

    if(m_textLines.length() > 0)
    {
      for(int i=0;i<m_textLines.length();i++)
      {
        float lineW = (float)m_metrics->width(m_textLines[i]) + 10.0f;
        if(lineW > width)
          width = lineW;
        height += float(m_metrics->lineSpacing());
      }
    }
  }

  setMinimumWidth(width);
  setMaximumWidth(width);
  setMinimumHeight(height);
  setMaximumHeight(height);

  onNodePositionChanged(m_node, m_node->topLeftGraphPos());
}
