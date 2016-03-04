// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGTabSearchWidget.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGLogWidget.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/Dialogs/DFGNewVariableDialog.h>

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
  , m_queryMetrics( config.searchQueryFont )
  , m_resultsMetrics( config.searchResultsFont )
  , m_helpMetrics( config.searchHelpFont )
{
  // always show on top
  setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
  setMouseTracking(true);
}

DFGTabSearchWidget::~DFGTabSearchWidget()
{
  releaseKeyboard();
}

void DFGTabSearchWidget::mousePressEvent(QMouseEvent * event)
{
  // If we get a left click
  if(event->button() == Qt::LeftButton)
  {
    // Get the element index from the click pos
    int index = indexFromPos( event->pos() );
    if ( index >= 0 )
    {
      // Then add the node to the graph
      addNodeFromPath( m_results[index]);
      m_parent->getGraphViewWidget()->setFocus(Qt::OtherFocusReason);
      event->accept();
      return;
    }
  }
  QWidget::mousePressEvent(event);
}

void DFGTabSearchWidget::mouseMoveEvent( QMouseEvent *event )
{
  int index = indexFromPos( event->pos() );
  if ( m_currentIndex != index )
  {
    m_currentIndex = index;
    update();
  }
  event->accept();
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
      addNodeFromPath(m_results[m_currentIndex]);
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

char const *DFGTabSearchWidget::getHelpText() const
{
  if ( m_search.length() == 0 )
    return "Search for preset or variable";
  else if ( m_results.length() == 0 )
    return "No results found";
  else
    return 0;
}

void DFGTabSearchWidget::paintEvent(QPaintEvent * event)
{
  QPainter painter(this);

  int width = widthFromResults();
  int height = heightFromResults();

  painter.fillRect(0, 0, width, height, m_config.searchBackgroundColor);
  painter.fillRect(
    margin() + m_queryMetrics.width( m_search ),
    margin(),
    m_queryMetrics.width( 'X' ),
    m_queryMetrics.lineSpacing(),
    m_config.searchCursorColor
    );

  if(m_currentIndex > -1 && m_currentIndex < m_results.length())
  {
    int offset = m_resultsMetrics.lineSpacing() * (m_currentIndex + 1) + margin();
    painter.fillRect(margin(), offset, width - 2 * margin(), m_resultsMetrics.lineSpacing(), m_config.searchHighlightColor);
  }

  painter.setPen(m_config.searchBackgroundColor.darker());
  painter.drawRect(0, 0, width-1, height-1);

  int offset = margin() + m_queryMetrics.ascent();
  painter.setFont(m_config.searchQueryFont);
  painter.setPen(QColor(0, 0, 0, 255));
  painter.drawText(margin(), offset, m_search);
  offset += m_queryMetrics.lineSpacing();

  painter.setFont(m_config.searchResultsFont);
  for(int i=0;i<m_results.length();i++)
  {
    painter.drawText(margin(), offset, resultLabel(i));
    offset += m_resultsMetrics.lineSpacing();
  }

  if ( char const *helpText = getHelpText() )
  {
    painter.setFont(m_config.searchHelpFont);
    painter.drawText(margin(), offset, helpText);
    offset += m_helpMetrics.lineSpacing();
  }

  QWidget::paintEvent(event);  
}

void DFGTabSearchWidget::hideEvent(QHideEvent * event)
{
  releaseKeyboard();
  emit enabled(false);
  QWidget::hideEvent(event);  
}

void DFGTabSearchWidget::showForSearch( QPoint globalPos )
{
  m_results.clear();
  m_search.clear();
  m_currentIndex = -1;
  setFocus(Qt::TabFocusReason);

  QPoint localPos = m_parent->mapFromGlobal( globalPos );
  setGeometry( int( localPos.x() - width() * 0.5 ),
               int( localPos.y() - m_queryMetrics.lineSpacing() * 0.5 ), 0,
               0 );
  updateGeometry();

  emit enabled(true);
  show();

  grabKeyboard();
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
  y -= m_queryMetrics.lineSpacing();
  y /= (int)m_resultsMetrics.lineSpacing();
  if ( y < 0 || y >= m_results.length() )
    return -1;
  return y;
}

int DFGTabSearchWidget::widthFromResults() const
{
  int width = 80;

  int w = m_queryMetrics.width(m_search) + m_queryMetrics.width('X');
  if(w > width)
    width = w;

  for(int i=0;i<m_results.length();i++)
  {
    w = m_resultsMetrics.width(resultLabel(i));
    if(w > width)
      width = w;
  }

  if ( char const *helpText = getHelpText() )
  {
    w = m_helpMetrics.width( helpText );
    if ( w > width )
      width = w;
  }

  return width + 2 * margin();
}

int DFGTabSearchWidget::heightFromResults() const
{
  int height = m_queryMetrics.lineSpacing();
  height += m_results.length() * m_resultsMetrics.lineSpacing();
  if ( getHelpText() )
    height += m_helpMetrics.lineSpacing();
  return height + 2 * margin();
}

void DFGTabSearchWidget::addNodeFromPath(QString path)
{
  DFGController *controller = m_parent->getUIController();
  
  QPoint localPos = geometry().topLeft();
  QPointF scenePos = m_parent->getGraphViewWidget()->graph()->itemGroup()->mapFromScene(localPos);

  // init node name.
  QString nodeName;

  // deal with special case
  if(path == "var")
  {
    FabricCore::Client client = controller->getClient();
    FabricCore::DFGBinding binding = controller->getBinding();

    DFGNewVariableDialog dialog(
      this, client, binding, controller->getExecPath()
      );
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString name = dialog.name();
    QString dataType = dialog.dataType();
    QString extension = dialog.extension();

    nodeName = controller->cmdAddVar(
      name.toUtf8().constData(), 
      dataType.toUtf8().constData(), 
      extension.toUtf8().constData(), 
      scenePos
      );
  }
  else if(path == "get")
  {
    nodeName = controller->cmdAddGet(
      "get",
      "",
      scenePos
      );
  }
  else if(path == "set")
  {
    nodeName = controller->cmdAddSet(
      "set",
      "",
      scenePos
      );
  }
  else if(path.left(4) == "get.")
  {
    nodeName = controller->cmdAddGet(
      "get",
      path.mid(4).toUtf8().constData(),
      scenePos
      );
  }
  else if(path.left(4) == "set.")
  {
    nodeName = controller->cmdAddSet(
      "set",
      path.mid(4).toUtf8().constData(),
      scenePos
      );
  }
  else
  {
    nodeName = controller->cmdAddInstFromPreset(
      path.toUtf8().constData(),
      scenePos
      ).toUtf8().constData();
  }

  // was a new node created?
  if ( !nodeName.isEmpty() )
  {
    m_parent->getGraphViewWidget()->graph()->clearSelection();
    if ( GraphView::Node *uiNode = m_parent->getGraphViewWidget()->graph()->node( nodeName ) )
      uiNode->setSelected( true );
  }
}
