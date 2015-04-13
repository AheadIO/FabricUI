// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "GraphViewWidget.h"

#include <QtGui/QPainter>

using namespace FabricUI::GraphView;

GraphViewWidget::GraphViewWidget(QWidget * parent, const GraphConfig & config, GraphFactory * factory, Graph * graph)
: QGraphicsView(parent)
{
  setRenderHint(QPainter::Antialiasing);
  setRenderHint(QPainter::HighQualityAntialiasing);
  setRenderHint(QPainter::TextAntialiasing);

  setStyleSheet( "QGraphicsView { border-style: none; }" );

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  setCacheMode(CacheBackground);
  setViewportUpdateMode(SmartViewportUpdate);

  m_scene = new QGraphicsScene();
  setScene(m_scene);

  m_graph = graph;
  if(!m_graph)
    m_graph = new Graph(NULL, config, factory);
  m_scene->addItem(m_graph);

  setAcceptDrops(true);
  setMouseTracking(true);
}

Graph * GraphViewWidget::graph()
{
  return m_graph;
}

const Graph * GraphViewWidget::graph() const
{
  return m_graph;
}

void GraphViewWidget::resizeEvent(QResizeEvent * event)
{
  setSceneRect(0, 0, event->size().width(), event->size().height());
  m_graph->setGeometry(0, 0, event->size().width(), event->size().height());
}

void GraphViewWidget::dragMoveEvent(QDragMoveEvent *event)
{
  event->accept();
}

void GraphViewWidget::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat("text/plain"))
  {
    event->acceptProposedAction();
  }
}

void GraphViewWidget::dropEvent(QDropEvent *event)
{
  m_lastEventPos = event->pos();

  // event->mimeData()->text()
  QString preset = event->mimeData()->text();

  graph()->controller()->beginInteraction();

  QPointF pos(event->pos().x(), event->pos().y());
  pos = graph()->itemGroup()->mapFromScene(pos);

  Node * node = graph()->controller()->addNodeFromPreset(preset, pos);

  graph()->controller()->endInteraction();

  if(node)
    event->acceptProposedAction();
}

void GraphViewWidget::mouseMoveEvent(QMouseEvent * event)
{
  m_lastEventPos = event->pos();
  QGraphicsView::mouseMoveEvent(event);
}

void GraphViewWidget::keyPressEvent(QKeyEvent * event)
{
  if(graph()->pressHotkey((Qt::Key)event->key(), (Qt::KeyboardModifier)(int)event->modifiers()))
    event->accept();
  else
    QGraphicsView::keyPressEvent(event);
}

void GraphViewWidget::keyReleaseEvent(QKeyEvent * event)
{
  if(graph()->releaseHotkey((Qt::Key)event->key(), (Qt::KeyboardModifier)(int)event->modifiers()))
    event->accept();
  else
    QGraphicsView::keyPressEvent(event);
}

QPoint GraphViewWidget::lastEventPos() const
{
  return m_lastEventPos;
}

bool GraphViewWidget::focusNextPrevChild(bool next)
{
  // avoid focus switching
  return false;
}
