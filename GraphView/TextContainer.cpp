// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "TextContainer.h"
#include <QtGui/QPen>
#include <QtGui/QFontMetrics>

using namespace FabricUI::GraphView;

TextContainer::TextContainer(
  QGraphicsWidget * parent,
  QString const &text,
  QColor color,
  QColor hlColor,
  QFont font
  )
: QGraphicsWidget(parent)
{
  m_color = color;
  m_font = font;
  m_highlightColor = hlColor;
  m_highlighted = false;
  
  m_textItem = new QGraphicsSimpleTextItem(text, this);
  m_textItem->setPen(QPen(Qt::NoPen));
  m_textItem->setBrush(color);
  m_textItem->setFont(font);

  setWindowFrameMargins(0, 0, 0, 0);
  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

  refresh();
}

void TextContainer::setText(QString const &text)
{
  m_textItem->setText(text);
  refresh();
}

void TextContainer::refresh()
{
  QFontMetrics metrics( m_font );
  QSize size = metrics.size( Qt::TextSingleLine, m_textItem->text() );
  prepareGeometryChange();
  setPreferredWidth(size.width());
  setPreferredHeight(size.height());

  m_textItem->update();
}

QColor TextContainer::color() const
{
  return m_color;
}

QColor TextContainer::highlightColor() const
{
  return m_highlightColor;
}

void TextContainer::setColor(QColor color, QColor hlColor)
{
  m_color = color;
  m_highlightColor = hlColor;
  if(highlighted())
  {
    m_textItem->setBrush(hlColor);
  }
  else
    m_textItem->setBrush(color);
}

bool TextContainer::highlighted() const
{
  return m_highlighted;
}

void TextContainer::setHighlighted(bool state)
{
  m_highlighted = state;
  setColor(m_color, m_highlightColor);
}

QFont TextContainer::font() const
{
  return m_textItem->font();
}

void TextContainer::setFont(QFont font)
{
  m_textItem->setFont(font);
  refresh();
}

bool TextContainer::italic() const
{
  return m_textItem->font().italic();
}

void TextContainer::setItalic(bool flag)
{
  QFont font = m_textItem->font();
  font.setItalic(flag);
  setFont(font);
}

QGraphicsSimpleTextItem * TextContainer::textItem()
{
  return m_textItem;
}
