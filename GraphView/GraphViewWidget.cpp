// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "GraphViewWidget.h"

#include <QtGui/QPainter>
#include <QtOpenGL/QGLWidget>

#ifdef FABRICUI_TIMERS
  #include <Util/Timer.h>
#endif

using namespace FabricUI::GraphView;

GraphViewWidget::GraphViewWidget(
  QWidget * parent,
  const GraphConfig & config,
  Graph * graph
  )
  : QGraphicsView(parent)
{
  setRenderHint(QPainter::Antialiasing);
  // setRenderHint(QPainter::HighQualityAntialiasing);
  setRenderHint(QPainter::TextAntialiasing);
  setOptimizationFlag(DontSavePainterState);

  setStyleSheet( "QGraphicsView { border-style: none; }" );

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  setBackgroundBrush(config.mainPanelBackgroundColor);

  setViewportUpdateMode(SmartViewportUpdate);

  // use opengl for rendering with multi sampling
  if(config.useOpenGL)
  {
    QGLFormat format;
    format.setSampleBuffers(true);
    QGLContext * context = new QGLContext(format);
    QGLWidget * glWidget = new QGLWidget(context);
    setViewport(glWidget);
  }

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
  m_scene = new GraphViewScene( graph );
  setScene(m_scene);

  QObject::connect(m_scene, SIGNAL(changed(const QList<QRectF> &)), this, SLOT(onSceneChanged()));

  m_graph = graph;
  if(m_graph)
  {
    m_graph->setGeometry(0, 0, size().width(), size().height());
    m_scene->addItem(m_graph);
    m_graph->updateOverlays(rect().width(), rect().height());
  }
}

void GraphViewWidget::resizeEvent(QResizeEvent * event)
{
  setSceneRect(0, 0, event->size().width(), event->size().height());
  if (m_graph)
  {
    m_graph->setGeometry(0, 0, event->size().width(), event->size().height());
    m_graph->updateOverlays(event->size().width(), event->size().height());
  }
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
  QString presetQString = event->mimeData()->text();
  FTL::CStrRef presetCStr = presetQString.toUtf8().constData();

  QPointF pos( event->pos().x(), event->pos().y() );
  pos = graph()->itemGroup()->mapFromScene( pos );

  if ( graph()->controller()->gvcDoAddInstFromPreset( presetCStr, pos ) )
    event->acceptProposedAction();
}

void GraphViewWidget::mouseMoveEvent(QMouseEvent * event)
{
  m_lastEventPos = event->pos();
  QGraphicsView::mouseMoveEvent(event);
}

void GraphViewWidget::keyPressEvent(QKeyEvent * event)
{
  if(!event->isAutoRepeat() && graph()->pressHotkey((Qt::Key)event->key(), (Qt::KeyboardModifier)(int)event->modifiers()))
    event->accept();
  else
    QGraphicsView::keyPressEvent(event);
}

void GraphViewWidget::keyReleaseEvent(QKeyEvent * event)
{
  if(!event->isAutoRepeat() && graph()->releaseHotkey((Qt::Key)event->key(), (Qt::KeyboardModifier)(int)event->modifiers()))
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

GraphViewScene::GraphViewScene( Graph * graph ) {
  m_graph = graph;
}

// Enable the MainPanel to grab specific events
// (eg: panning with Space or Alt) before these
// are redirected to sub-widgets.
bool GraphViewScene::event( QEvent * e ) {
  if( m_graph && m_graph->mainPanel()->grabsEvent( e ) ) {
    return sendEvent( m_graph->mainPanel(), e );
  } else
    return QGraphicsScene::event( e );
}
