// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "LineNumberWidget.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

#include <stdio.h>

using namespace FabricUI::KLEditor;

LineNumberWidget::LineNumberWidget(QWidget * parent, const EditorConfig & config)
: QWidget(parent)
{
  m_config = config;
  m_metrics = new QFontMetrics(m_config.lineNumberFont);
  m_lineOffset = 0;

  int maxWidth = m_metrics->width("0000") + 4;
  setMinimumWidth(maxWidth);
  setMaximumWidth(maxWidth);

  setContentsMargins(0, 0, 0, 0);
}

LineNumberWidget::~LineNumberWidget()
{
  delete(m_metrics);
}

unsigned int LineNumberWidget::lineOffset() const
{
  return m_lineOffset;
}

void LineNumberWidget::setLineOffset(unsigned int offset)
{
  if(offset != m_lineOffset)
  {
    m_lineOffset = offset;
    update();
  }
}

void LineNumberWidget::paintEvent(QPaintEvent * event)
{
  QPainter painter(this);
  
  painter.fillRect(event->rect(), m_config.lineNumberBackgroundColor);

  int width = event->rect().width();
  int height = event->rect().height();

#ifdef FABRIC_OS_LINUX
  // [andrew 20150907] Courier New has a leading() value of -1 on Linux which doesn't
  // line up with the real line spacing seen in the source code widget
  int lineHeight = m_metrics->height();
  int offset = lineHeight;
  if(m_lineOffset == 0)
    offset += 1;
  else
    offset -= 3;
#else
  int lineHeight = m_metrics->height() + m_metrics->leading() * 2;
  int offset = lineHeight + 1;
  if(m_lineOffset == 0)
    offset += 1;
  else
    offset -= 2;
#endif

  painter.setFont(m_config.lineNumberFont);
  painter.setPen(m_config.lineNumberFontColor);

  int line = m_lineOffset + 1;
  char buffer[128];
  while(offset < height)
  {
    std::string paddingNumber;
#ifdef _WIN32
    itoa(line, buffer, 10);
#else
    snprintf(buffer, 128, "%d", line);
#endif
    paddingNumber = buffer;
    while(paddingNumber.length() < 4)
      paddingNumber = "0" + paddingNumber;

    int lineWidth = m_metrics->width(paddingNumber.c_str());
    painter.drawText(QPoint(width - 2 - lineWidth, offset), paddingNumber.c_str());
    offset += lineHeight;
    line++;
  }

  QWidget::paintEvent(event);
}
