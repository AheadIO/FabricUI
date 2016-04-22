// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "GraphViewWidget.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtOpenGL/QGLWidget>

#ifdef FABRICUI_TIMERS
  #include <Util/Timer.h>
#endif

#include <stdlib.h>

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

  setAcceptDrops( true );

  // use opengl for rendering with multi sampling
  if(config.useOpenGL)
  {
    char const *useCanvasOpenGL = ::getenv( "FABRIC_USE_CANVAS_OPENGL" );
    if ( !!useCanvasOpenGL && !!useCanvasOpenGL[0] )
    {
      QGLFormat format;
      format.setSampleBuffers(true);
      QGLContext * context = new QGLContext(format);
      QGLWidget * glWidget = new QGLWidget(context);
      setViewport(glWidget);
    }
  }

  setGraph(graph);

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
  QObject::connect(
    m_scene, SIGNAL(urlDropped(QUrl, bool)),
    this, SIGNAL(urlDropped(QUrl, bool))
    );

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

void GraphViewScene::dragEnterEvent( QGraphicsSceneDragDropEvent *event )
{
  QMimeData const *mimeData = event->mimeData();
  if ( mimeData->hasUrls() )
  {
    QList<QUrl> urls = mimeData->urls();
    if ( urls.count() == 1 )
    {
      event->acceptProposedAction();
    }
  }

  if ( !event->isAccepted() )
    QGraphicsScene::dragEnterEvent( event );
}

void GraphViewScene::dropEvent( QGraphicsSceneDragDropEvent *event )
{
  QGraphicsScene::dropEvent( event );
  
  QMimeData const *mimeData = event->mimeData();
  if ( mimeData->hasUrls() )
  {
    QList<QUrl> urls = mimeData->urls();
    if ( urls.count() == 1 )
    {
      QUrl url = urls.front();

      bool bypassUnsavedChanges =
        event->modifiers().testFlag( Qt::ControlModifier );

      emit urlDropped( url, bypassUnsavedChanges );
    }
  }
}
