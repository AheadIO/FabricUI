// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGTabSearchWidget.h"
#include "DFGWidget.h"

#include <QtGui/QCursor>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGTabSearchWidget::DFGTabSearchWidget(DFGWidget * parent, const DFGConfig & config)
: QWidget(parent)
{
  m_config = config;
  m_parent = parent;
  m_metrics = new QFontMetrics(m_config.fixedFont);
  setMouseTracking(true);
  m_requiresUpdate = false;
  // requestPixmapUpdate();
}

DFGTabSearchWidget::~DFGTabSearchWidget()
{
  delete(m_metrics);
}

void DFGTabSearchWidget::mousePressEvent(QMouseEvent * event)
{
  if(event->button() == Qt::LeftButton)
  {
    int index = indexFromPos(event->pos());
    if(index >= 0)
    {
      QString graphPath = m_parent->getUIController()->graph()->path();
      QPoint localPos = m_parent->getGraphViewWidget()->mapFromGlobal(m_pos);
      QPointF scenePos = m_parent->getGraphViewWidget()->graph()->itemGroup()->mapFromScene(localPos);
      m_parent->getUIController()->addNodeFromPreset(graphPath, m_results[index], scenePos);

      hide();
      event->accept();
      m_parent->getGraphViewWidget()->setFocus(Qt::OtherFocusReason);
      return;
    }
  }
  QWidget::mousePressEvent(event);
}

void DFGTabSearchWidget::mouseMoveEvent(QMouseEvent * event)
{
  int index = indexFromPos(event->pos());
  if(index >= 0)
  {
    if(m_currentIndex != index)
    {
      m_currentIndex = index;
      m_requiresUpdate = true;
    }
    event->accept();
    return;
  }
  QWidget::mouseMoveEvent(event);
}

void DFGTabSearchWidget::keyPressEvent(QKeyEvent * event)
{
  Qt::Key key = (Qt::Key)event->key();
  if(key == Qt::Key_Tab || key == Qt::Key_Escape)
  {
    hide();
    event->accept();
    m_parent->getGraphViewWidget()->setFocus(Qt::OtherFocusReason);
  }
  // alphanumeric or period
  else if((int(key) >= int(Qt::Key_0) && int(key) <= int(Qt::Key_9)) ||
    (int(key) >= int(Qt::Key_A) && int(key) <= int(Qt::Key_Z)) ||
    key == Qt::Key_Period || key == Qt::Key_Underscore)
  {
    m_search += event->text();
    updateSearch();
    event->accept();
  }
  else if(key == Qt::Key_Backspace)
  {
    if(m_search.length() > 0)
    {
      m_search = m_search.left(m_search.length()-1);
      updateSearch();
    }
    event->accept();
  }
  else if(key == Qt::Key_Up)
  {
    if(m_currentIndex > 0)
    {
      m_currentIndex--;
      m_requiresUpdate = true;
      requestPixmapUpdate();
    }
    event->accept();
  }
  else if(key == Qt::Key_Down)
  {
    if(m_currentIndex < m_results.length() -1)
    {
      m_currentIndex++;
      requestPixmapUpdate();
    }
    event->accept();
  }
  else if(key == Qt::Key_Enter || key == Qt::Key_Return)
  {
    if(m_currentIndex > -1 && m_currentIndex < m_results.length())
    {
      QString graphPath = m_parent->getUIController()->graph()->path();
      QPoint localPos = m_parent->getGraphViewWidget()->mapFromGlobal(m_pos);
      QPointF scenePos = m_parent->getGraphViewWidget()->graph()->itemGroup()->mapFromScene(localPos);
      m_parent->getUIController()->addNodeFromPreset(graphPath, m_results[m_currentIndex], scenePos);
    }
    hide();
    event->accept();
    m_parent->getGraphViewWidget()->setFocus(Qt::OtherFocusReason);
  }
  else
  {
    QWidget::keyPressEvent(event);
  }
}

void DFGTabSearchWidget::paintEvent(QPaintEvent * event)
{
  updatePixmap();
  QPainter painter(this);
  painter.drawPixmap(0, 0, m_pixmap);
  QWidget::paintEvent(event);  
}

void DFGTabSearchWidget::showForSearch(QPoint pos)
{
  m_results.clear();
  m_search.clear();
  m_currentIndex = -1;
  setFocus(Qt::TabFocusReason);
  m_pos = QPoint(pos.x() - width() * 0.5, pos.y() - m_metrics->lineSpacing() * 0.5);
  requestPixmapUpdate();

  // always show on top
  Qt::WindowFlags flags = windowFlags();
  setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);

  show();
}

void DFGTabSearchWidget::showForSearch()
{
  showForSearch(QCursor::pos());
}

void DFGTabSearchWidget::focusOutEvent(QFocusEvent * event)
{
  hide();
}

bool DFGTabSearchWidget::focusNextPrevChild(bool next)
{
  // avoid focus switching
  return false;
}

void DFGTabSearchWidget::updateSearch()
{
  m_results = m_parent->getUIController()->getPresetPathsFromSearch(m_search);
  if(m_results.size() > 16)
  {
    QStringList lessResults;
    for(unsigned int i=0;i<16;i++)
      lessResults.push_back(m_results[i]);
    m_results = lessResults;
  }

  if(m_results.length() == 0)
  {
    m_currentIndex = -1;
  }
  else if(m_currentIndex == -1)
  {
    m_currentIndex = 0;
  }
  else if(m_currentIndex >= m_results.length())
  {
    m_currentIndex = 0;
  }
  requestPixmapUpdate();
}

void DFGTabSearchWidget::updatePixmap()
{
  if(!m_requiresUpdate)
    return;
  m_requiresUpdate = false;

  int width = widthFromResults();
  int height = heightFromResults();

  m_pixmap = QPixmap(width, height);

  QPainter painter(&m_pixmap);
  painter.fillRect(0, 0, m_pixmap.width(), m_pixmap.height(), m_config.searchBackgroundColor);

  if(m_currentIndex > -1 && m_currentIndex < m_results.length())
  {
    int offset = m_metrics->lineSpacing() * (m_currentIndex + 1) + margin();
    painter.fillRect(margin(), offset, m_pixmap.width() - 2 * margin(), m_metrics->lineSpacing(), m_config.searchHighlightColor);
  }

  painter.setPen(m_config.searchBackgroundColor.darker());
  painter.drawLine(QPoint(0, 0), QPoint(m_pixmap.width()-1, 0));
  painter.drawLine(QPoint(0, 0), QPoint(0, m_pixmap.height()-1));
  painter.setPen(m_config.searchBackgroundColor.lighter());
  painter.drawLine(QPoint(m_pixmap.width()-1, 0), QPoint(m_pixmap.width()-1, m_pixmap.height()-1));
  painter.drawLine(QPoint(0, m_pixmap.height()-1), QPoint(m_pixmap.width()-1, m_pixmap.height()-1));

  painter.setFont(m_config.fixedFont);
  painter.setPen(QColor(0, 0, 0, 255));

  int offset = margin() + m_metrics->ascent();
  painter.drawText(margin(), offset, m_search);

  for(unsigned int i=0;i<m_results.length();i++)
  {
    offset += m_metrics->lineSpacing();
    painter.drawText(margin(), offset, resultLabel(i));
  }

  show();
}

int DFGTabSearchWidget::margin() const
{
  return 2;
}

void DFGTabSearchWidget::requestPixmapUpdate()
{
  int width = widthFromResults();
  int height = heightFromResults();

  QPoint pos = m_parent->mapFromGlobal(m_pos);
  
  QWidget * parentWidget = qobject_cast<QWidget*>(parent());
  if(parentWidget)
  {
    QPoint tl = pos;
    QPoint br = pos + QPoint(width, height);

    if(tl.x() < 0)
      pos += QPoint(-tl.x(), 0);
    else if(br.x() > parentWidget->width())
      pos -= QPoint(br.x() - parentWidget->width(), 0);
    if(tl.y() < 0)
      pos += QPoint(0, -tl.y());
    else if(br.y() > parentWidget->height())
      pos -= QPoint(0, br.y() - parentWidget->height());
  }

  setGeometry(pos.x(), pos.y(), width, height);

  m_requiresUpdate = true;
}

QString DFGTabSearchWidget::resultLabel(unsigned int index) const
{
  int periodPos = m_results[index].lastIndexOf('.');
  if(periodPos > 0)
  {
    periodPos = m_results[index].lastIndexOf('.', periodPos-1);
    if(periodPos > 0)
    {
      return "..."+m_results[index].right(m_results[index].length() - periodPos - 1);
    }
  }
  return m_results[index];
}

int DFGTabSearchWidget::indexFromPos(QPoint pos)
{
  int y = pos.y();
  y -= margin();
  y /= (int)m_metrics->lineSpacing();
  y -= 1;
  if(y < 0 || y > m_results.size())
    return -1;
  return y;
}

int DFGTabSearchWidget::widthFromResults() const
{
  int width = 80 + 2 * margin();

  int w = m_metrics->width(m_search) + 2 * margin();
  if(w > width)
    width = w;

  for(unsigned int i=0;i<m_results.length();i++)
  {
    w = m_metrics->width(resultLabel(i)) + 2 * margin();
    if(w > width)
      width = w;
  }

  return width;
}

int DFGTabSearchWidget::heightFromResults() const
{
  return (m_results.length() + 1) * m_metrics->lineSpacing() + 2 * margin();
}
