// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "GraphViewWidget.h"

#include <QtGui/QPainter>

#ifdef FABRICUI_TIMERS
  #include <Util/Timer.h>
#endif

using namespace FabricUI::GraphView;

GraphViewWidget::GraphViewWidget(QWidget * parent, const GraphConfig & config, GraphFactory * factory, Graph * graph)
: QGraphicsView(parent)
{
  setRenderHint(QPainter::Antialiasing);
  // setRenderHint(QPainter::HighQualityAntialiasing);
  setRenderHint(QPainter::TextAntialiasing);

  setStyleSheet( "QGraphicsView { border-style: none; }" );

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  setCacheMode(CacheBackground);

  setViewportUpdateMode(SmartViewportUpdate);
  // setViewportUpdateMode(MinimalViewportUpdate);

  setGraph(graph);

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

void GraphViewWidget::setGraph(Graph * graph)
{
  m_scene = new QGraphicsScene();
  setScene(m_scene);

  QObject::connect(m_scene, SIGNAL(changed(const QList<QRectF> &)), this, SLOT(onSceneChanged()));

  m_graph = graph;
  if(m_graph)
  {
    m_scene->addItem(m_graph);
    m_graph->setGeometry(0, 0, size().width(), size().height());
  }
}

void GraphViewWidget::resizeEvent(QResizeEvent * event)
{
  setSceneRect(0, 0, event->size().width(), event->size().height());
  if (m_graph)
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

void GraphViewWidget::onSceneChanged()
{
#ifdef FABRICUI_TIMERS
  Util::TimerPtr overAllTimer = Util::Timer::getTimer("FabricUI::GraphViewWidget");

  std::map<std::string, Util::TimerPtr> & timers = Util::Timer::getAllTimers();
  std::map<std::string, Util::TimerPtr>::iterator it;
  for(it = timers.begin(); it != timers.end(); it++)
  {
    QString message;
    message += it->second->title();
    if(message.left(8) != "FabricUI")
      continue;
    double elapsed = it->second->getElapsedMS();
    if(elapsed == 0.0)
      continue;
    message += " " + QString::number(elapsed, 'g', 3);
    message += "ms";
    printf("%s\n", message.toUtf8().constData());
    it->second->reset();
  }

  overAllTimer->resume();

#endif  
  emit sceneChanged();
}

bool GraphViewWidget::focusNextPrevChild(bool next)
{
  // avoid focus switching
  return false;
}
