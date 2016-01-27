// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "NodeRectangle.h"
#include "Node.h"
#include "Graph.h"

#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtCore/QDebug>

#ifdef FABRICUI_TIMERS
  #include <Util/Timer.h>
#endif

using namespace FabricUI::GraphView;

NodeRectangle::NodeRectangle(Node * parent)
: QGraphicsWidget(parent)
{
  m_node = parent;
}

void NodeRectangle::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
#ifdef FABRICUI_TIMERS
  Util::TimerPtr timer = Util::Timer::getTimer("FabricUI::NodeRectangle");
  timer->resume();
#endif

  QPen standardPen = m_node->m_defaultPen;
  if(m_node->selected())
    standardPen = m_node->m_selectedPen;
  if(m_node->hasError())
    standardPen = m_node->m_errorPen;

  QRectF rect = windowFrameRect();

  float nodeWidthReduction = m_node->graph()->config().nodeWidthReduction * 0.5;
  rect.adjust(nodeWidthReduction, standardPen.width() * 0.5f, -nodeWidthReduction, -standardPen.width() * 0.5f);

  QLinearGradient gradient(0.5, 0.0, 0.5, 1.0);
  gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
  gradient.setColorAt(0.0, m_node->m_colorA);
  gradient.setColorAt(1.0, m_node->m_colorB);

  painter->setBrush(gradient);

  // painter->setRenderHint(QPainter::Antialiasing,true);
  // painter->setRenderHint(QPainter::HighQualityAntialiasing,true);

  QPainterPath rounded_rect;
  rounded_rect.addRoundRect(
    rect, int( 150.0f * m_node->m_cornerRadius / rect.width() ),
    int( 150.0f * m_node->m_cornerRadius / rect.height() ) );

  // fill everything
  painter->fillPath(rounded_rect,painter->brush());     

  // draw the label
  QRectF labelRect(rect.left(), rect.top(), rect.width(), m_node->m_header->size().height());
  painter->setClipPath(rounded_rect);
  painter->setClipRect(labelRect, Qt::IntersectClip);
  painter->setBrush(m_node->m_titleColor);
  painter->fillPath(rounded_rect,painter->brush());     

  // remove the clipping
  painter->setClipRect(rect.adjusted(-2, -2, 2, 2));

  // draw the header lines
  // FE-4157
  // QPen coloredPen = standardPen;
  // coloredPen.setColor(m_node->color());
  // painter->setPen(coloredPen);
  // painter->drawLine(labelRect.bottomLeft() - QPointF(0, standardPen.width()), labelRect.bottomRight() - QPointF(0, standardPen.width()));
  painter->setPen(standardPen);
  painter->drawLine(labelRect.bottomLeft(), labelRect.bottomRight());

  // draw the outline
  painter->strokePath(rounded_rect, standardPen);

#ifdef FABRICUI_TIMERS
  timer->pause();
#endif

  painter->setClipping(false);
  QGraphicsWidget::paint(painter, option, widget);

}
