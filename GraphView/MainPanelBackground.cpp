// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "MainPanelBackground.h"
#include "MainPanel.h"
#include "Graph.h"
#include "GraphConfig.h"

#include <QtGui/QPainter>

using namespace FabricUI::GraphView;

MainPanelBackground::MainPanelBackground(MainPanel * parent)
: QGraphicsWidget(parent)
{
  m_panel = parent;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

const QTransform & MainPanelBackground::offset() const
{
  return m_offset;
}

void MainPanelBackground::setOffset(QTransform transform)
{
  if(m_offset != transform)
  {
    m_offset = transform;
    update();
  }
}

void MainPanelBackground::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  QRectF rect = m_panel->windowFrameRect();
  painter->fillRect(rect, m_panel->graph()->config().mainPanelBackgroundColor);

  if(m_panel->graph()->config().mainPanelDrawGrid)
  {
    rect = m_offset.inverted().mapRect(rect);
    
    QTransform oldTransform = painter->transform();
    painter->setTransform(offset(), true);

    int gridSize = m_panel->graph()->config().mainPanelGridSpanS;
    int left = int(rect.left()) - (int(rect.left()) % gridSize);
    int top = int(rect.top()) - (int(rect.top()) % gridSize);

    // todo: allocate this first, the count is fixed
    std::vector<QLineF> gridLines;

    // Draw horizontal fine lines 
    gridLines.clear();
    painter->setPen(m_panel->graph()->config().mainPanelGridPenS);
    float y = float(top);
    while(y < float(rect.bottom()))
    {
      gridLines.push_back(QLineF( rect.left(), y, rect.right(), y ));
      y += float(gridSize);
    }
    painter->drawLines(&gridLines[0], gridLines.size());

    // Draw vertical fine lines 
    gridLines.clear();
    float x = float(left);
    while(x < float(rect.right()))
    {
      gridLines.push_back(QLineF( x, rect.top(), x, rect.bottom() ));
      x += float(gridSize);
    }
    painter->drawLines(&gridLines[0], gridLines.size());

    gridSize = m_panel->graph()->config().mainPanelGridSpanL;
    left = int(rect.left()) - (int(rect.left()) % gridSize);
    top = int(rect.top()) - (int(rect.top()) % gridSize);

    // Draw horizontal thick lines 
    gridLines.clear();
    painter->setPen(m_panel->graph()->config().mainPanelGridPenL);
    y = float(top);
    while(y < float(rect.bottom()))
    {
      gridLines.push_back(QLineF( rect.left(), y, rect.right(), y ));
      y += float(gridSize);
    }
    painter->drawLines(&gridLines[0], gridLines.size());

    // Draw vertical thick lines 
    gridLines.clear();
    x = float(left);
    while(x < float(rect.right()))
    {
      gridLines.push_back(QLineF( x, rect.top(), x, rect.bottom() ));
      x += float(gridSize);
    }
    painter->drawLines(&gridLines[0], gridLines.size());

    painter->setTransform(oldTransform);
  }

  QGraphicsWidget::paint(painter, option, widget);
}

