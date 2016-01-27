// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "SelectionRect.h"
#include "MainPanel.h"
#include "Node.h"
#include "Graph.h"

#include <QtGui/QPainter>

using namespace FabricUI::GraphView;

SelectionRect::SelectionRect(MainPanel * parent, QPointF mouseDownPos)
: QGraphicsWidget(parent->itemGroup())
{
  m_mainPanel = parent;
  m_color = graph()->config().selectionRectColor;
  m_pen = graph()->config().selectionRectPen;
  m_mouseDownPos = mouseDownPos;

  setZValue(-1);
  setPos(m_mouseDownPos);
  resize(0, 0);
}

Graph * SelectionRect::graph()
{
  return m_mainPanel->graph();
}

const Graph * SelectionRect::graph() const
{
  return m_mainPanel->graph();
}

void SelectionRect::setDragPoint(QPointF dragPoint)
{
  QPointF topLeft = m_mouseDownPos;
  QPointF bottomRight = dragPoint;
  if(dragPoint.x() < m_mouseDownPos.x())
  {
    topLeft.setX(dragPoint.x());
    bottomRight.setX(m_mouseDownPos.x());
  }
  if(dragPoint.y() < m_mouseDownPos.y())
  {
    topLeft.setY(dragPoint.y());
    bottomRight.setY(m_mouseDownPos.y());
  }
  setPos(topLeft);
  resize(bottomRight.x() - topLeft.x(), bottomRight.y() - topLeft.y());
}

void SelectionRect::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  QRectF rect = windowFrameRect();
  painter->setBrush(m_color);
  painter->setPen(m_pen);
  painter->drawRect(rect);
}
