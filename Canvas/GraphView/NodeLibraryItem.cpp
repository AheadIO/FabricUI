// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NodeLibraryItem.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>

using namespace FabricUI::GraphView;

NodeLibraryItem::NodeLibraryItem(QWidget * parent, QString path, QString label, QColor color)
: QLabel(label, parent)
{
  m_path = path;
  m_color = color;
  m_highlighted = false;

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  setContentsMargins(2, 2, 2, 2);
  setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

  QPalette p = palette();
  p.setColor(QPalette::Window, m_color);
  setPalette(p);

  setBackgroundRole(QPalette::Window);
  setAutoFillBackground(true);
}

QString NodeLibraryItem::path() const
{
  return m_path;
}

QColor NodeLibraryItem::color() const
{
  return m_color;
}

void NodeLibraryItem::mousePressEvent(QMouseEvent * event)
{
  if (event->button() == Qt::LeftButton)
  {
    QDrag * drag = new QDrag(this);

    QMimeData * mimeData = new QMimeData;
    mimeData->setText(path());
    drag->setMimeData(mimeData);

    m_highlighted = false;

    QPixmap pixmap(size());
    render(&pixmap);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos());
  }

  QLabel::mousePressEvent(event);
}

void NodeLibraryItem::enterEvent(QEvent * event)
{
  m_highlighted = true;

  QPalette p = palette();
  p.setColor(QPalette::Window, m_color.lighter());
  setPalette(p);
}

void NodeLibraryItem::leaveEvent(QEvent * event)
{
  m_highlighted = false;

  QPalette p = palette();
  p.setColor(QPalette::Window, m_color);
  setPalette(p);
}
