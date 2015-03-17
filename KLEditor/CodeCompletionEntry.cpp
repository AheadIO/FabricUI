// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "CodeCompletionEntry.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

using namespace FabricUI::KLEditor;

CodeCompletionEntry::CodeCompletionEntry(QWidget * parent, QString prefix, QString suffix, QString desc, const EditorConfig & config)
: QWidget(parent)
{
  m_config = config;
  m_metrics = new QFontMetrics(m_config.codeCompletionFont);
  m_prefix = prefix;
  m_suffix = suffix;
  m_desc = desc;
  m_active = false;

  int margins = m_config.codeCompletionMargins;
  int maxHeight = m_metrics->height() + m_metrics->leading() + 2 * margins;
  setMinimumHeight(maxHeight);
  setMaximumHeight(maxHeight);

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

  setContentsMargins(margins, 0, margins, 0);

  int minWidth = m_metrics->width(label()) + 2 * margins;
  setMinimumWidth(minWidth);

}

CodeCompletionEntry::~CodeCompletionEntry()
{
  delete(m_metrics);
}

QString CodeCompletionEntry::label() const
{
  return m_prefix + m_suffix + m_desc;
}

QString CodeCompletionEntry::prefix() const
{ 
  return m_prefix;
}

QString CodeCompletionEntry::suffix() const
{ 
  return m_suffix;
}

QString CodeCompletionEntry::desc() const
{ 
  return m_desc;
}

bool CodeCompletionEntry::active() const
{
  return m_active;
}

void CodeCompletionEntry::setActive(bool state)
{
  if(m_active != state)
  {
    m_active = state;
    update();
  }
}

void CodeCompletionEntry::paintEvent(QPaintEvent * event)
{
  QPainter painter(this);
  
  painter.fillRect(event->rect(), m_active ? m_config.codeCompletionActiveBackgroundColor : m_config.codeCompletionBackgroundColor);
  painter.setFont(m_config.codeCompletionFont);

  int margins = m_config.codeCompletionMargins;
  int height = m_metrics->lineSpacing() + margins - m_config.codeCompletionFontSize * 0.5;
  int offset = margins;
  
  painter.setPen(m_config.codeCompletionFontColor);
  painter.drawText(offset, height, m_prefix);

  offset += m_metrics->width(m_prefix);

  painter.setPen(m_config.codeCompletionFontColorSuffix);
  painter.drawText(offset, height, m_suffix + m_desc);

  QWidget::paintEvent(event);
}
