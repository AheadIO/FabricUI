// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGTabSearchWidget.h"
#include "DFGWidget.h"
#include "DFGLogWidget.h"

#include <QtGui/QCursor>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGTabSearchWidget::DFGTabSearchWidget(
  DFGWidget * parent,
  const DFGConfig & config
  )
  : QWidget(parent)
  , m_parent( parent )
  , m_config( config )
  , m_metrics( config.fixedFont )
{
  // always show on top
  setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
  setMouseTracking(true);
}

DFGTabSearchWidget::~DFGTabSearchWidget()
{
}

void DFGTabSearchWidget::mousePressEvent(QMouseEvent * event)
{
  if(event->button() == Qt::LeftButton)
  {
    int index = indexFromPos(event->pos());
    if(index >= 0)
    {
      QPoint localPos = geometry().topLeft();
      QPointF scenePos = m_parent->getGraphViewWidget()->graph()->itemGroup()->mapFromScene(localPos);
      m_parent->getUIController()->addNodeFromPreset(
        m_results[index].toUtf8().constData(), scenePos
        );

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
      update();
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
      update();
    }
    event->accept();
  }
  else if(key == Qt::Key_Down)
  {
    if(m_currentIndex < m_results.length() -1)
    {
      m_currentIndex++;
      update();
    }
    event->accept();
  }
  else if(key == Qt::Key_Enter || key == Qt::Key_Return)
  {
    if(m_currentIndex > -1 && m_currentIndex < m_results.length())
    {
      QPoint localPos = geometry().topLeft();
      QPointF scenePos = m_parent->getGraphViewWidget()->graph()->itemGroup()->mapFromScene(localPos);
      m_parent->getUIController()->addNodeFromPreset(
        m_results[m_currentIndex].toUtf8().constData(), scenePos
        );
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
  QPainter painter(this);

  int width = widthFromResults();
  int height = heightFromResults();

  painter.fillRect(0, 0, width, height, m_config.searchBackgroundColor);

  if(m_currentIndex > -1 && m_currentIndex < m_results.length())
  {
    int offset = m_metrics.lineSpacing() * (m_currentIndex + 1) + margin();
    painter.fillRect(margin(), offset, width - 2 * margin(), m_metrics.lineSpacing(), m_config.searchHighlightColor);
  }

  painter.setPen(m_config.searchBackgroundColor.darker());
  painter.drawLine(QPoint(0, 0), QPoint(width-1, 0));
  painter.drawLine(QPoint(0, 0), QPoint(0, height-1));
  painter.setPen(m_config.searchBackgroundColor.lighter());
  painter.drawLine(QPoint(width-1, 0), QPoint(width-1, height-1));
  painter.drawLine(QPoint(0, height-1), QPoint(width-1, height-1));

  painter.setFont(m_config.fixedFont);
  painter.setPen(QColor(0, 0, 0, 255));

  int offset = margin() + m_metrics.ascent();
  painter.drawText(margin(), offset, m_search);

  for(unsigned int i=0;i<m_results.length();i++)
  {
    offset += m_metrics.lineSpacing();
    painter.drawText(margin(), offset, resultLabel(i));
  }

  QWidget::paintEvent(event);  
}

void DFGTabSearchWidget::showForSearch( QPoint globalPos )
{
  m_results.clear();
  m_search.clear();
  m_currentIndex = -1;
  setFocus(Qt::TabFocusReason);

  QPoint localPos = m_parent->mapFromGlobal( globalPos );
  setGeometry(
    localPos.x() - width() * 0.5,
    localPos.y() - m_metrics.lineSpacing() * 0.5,
    0, 0
    );
  updateGeometry();

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
  m_results = m_parent->getUIController()->getPresetPathsFromSearch(m_search.toUtf8().constData());
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

  updateGeometry();
}

int DFGTabSearchWidget::margin() const
{
  return 2;
}

void DFGTabSearchWidget::updateGeometry()
{
  QRect rect = geometry();
  int width = widthFromResults();
  int height = heightFromResults();

  QPoint localPos = rect.topLeft();

  QWidget * parentWidget = qobject_cast<QWidget*>(parent());
  if(parentWidget)
  {
    QPoint tl = localPos;
    QPoint br = localPos + QPoint(width, height);

    if(tl.x() < 0)
      localPos += QPoint(-tl.x(), 0);
    else if(br.x() > parentWidget->width())
      localPos -= QPoint(br.x() - parentWidget->width(), 0);
    if(tl.y() < 0)
      localPos += QPoint(0, -tl.y());
    else if(br.y() > parentWidget->height())
      localPos -= QPoint(0, br.y() - parentWidget->height());
  }

  rect.setTopLeft( localPos );
  rect.setSize( QSize( width, height ) );

  setGeometry( rect );
  update();
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
  y /= (int)m_metrics.lineSpacing();
  y -= 1;
  if(y < 0 || y > m_results.size())
    return -1;
  return y;
}

int DFGTabSearchWidget::widthFromResults() const
{
  int width = 80 + 2 * margin();

  int w = m_metrics.width(m_search) + 2 * margin();
  if(w > width)
    width = w;

  for(unsigned int i=0;i<m_results.length();i++)
  {
    w = m_metrics.width(resultLabel(i)) + 2 * margin();
    if(w > width)
      width = w;
  }

  return width;
}

int DFGTabSearchWidget::heightFromResults() const
{
  return (m_results.length() + 1) * m_metrics.lineSpacing() + 2 * margin();
}
