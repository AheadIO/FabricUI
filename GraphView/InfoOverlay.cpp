// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "InfoOverlay.h"
#include "Graph.h"
#include "Controller.h"

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QGraphicsDropShadowEffect>
#include <QtCore/QDebug>

using namespace FabricUI::GraphView;

InfoOverlay::InfoOverlay(Graph * parent, QString text, const GraphConfig & config)
: QGraphicsWidget(parent->mainPanel())
, m_graph(parent)
, m_config(config)
{
  m_metrics = new QFontMetrics(m_config.infoOverlayFont);
  setText(text);
  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  updateSize();
}

InfoOverlay::~InfoOverlay()
{
  delete m_metrics;
}

Graph * InfoOverlay::graph()
{
  return m_graph;
}

const Graph * InfoOverlay::graph() const
{
  return m_graph;
}

QString InfoOverlay::text() const
{
  return m_text;
}

void InfoOverlay::setText(QString t)
{
  if(t == m_text)
    return;
  m_text = t;
  m_textLines.clear();
  if(m_text.length() > 0)
    m_textLines = m_text.split('\n');

  updateSize();
}

void InfoOverlay::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  painter->setPen(m_config.nodeDefaultPen);
  painter->setBrush(m_config.infoOverlayColor);

  QRectF rect = windowFrameRect();
  painter->drawRect(rect);

  if(m_textLines.length() > 0)
  {
    painter->setFont(m_config.infoOverlayFont);

    float offset = 0.0f;
    for(int i=0;i<m_textLines.length();i++)
    {
      offset += m_metrics->lineSpacing();
      painter->drawText(QPointF(4.0f, 4.0f + offset), m_textLines[i]);
    }
  }

  QGraphicsWidget::paint(painter, option, widget);
}

void InfoOverlay::updateSize()
{
  float width = 0.0;
  float height = 0.0;

  width = m_config.infoOverlayMinWidth;
  height = 2.0f + float(m_metrics->lineSpacing());

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

  setMinimumWidth(width);
  setMaximumWidth(width);
  setMinimumHeight(height);
  setMaximumHeight(height);
}
