/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */


#if defined(FTL_OS_DARWIN)
# include <CoreFoundation/CFURL.h>
#endif
#include <FabricUI/Viewports/QtToKLEvent.h>
#include <FabricUI/SceneHub/Editors/SHEditorWidget.h>
#include "FabricUI/SceneHub/Viewports/RTRGLViewportWidget.h"

using namespace FabricUI::SceneHub;
using namespace FabricUI::Viewports;

RTRGLViewportWidget::RTRGLViewportWidget(
  SHGLRenderer *shGLRenderer,
  SHGLScene *shGLScene,
  int viewportIndex, 
  QGLContext *qglContext, 
  QWidget *parent, 
  QGLWidget *share,
  QSettings *settings) 
  : ViewportWidget(&shGLRenderer->getClient(), QColor(), qglContext, parent, share, settings)
  , m_viewportIndex(viewportIndex)
  , m_alwaysRefresh(false)
  , m_orthographic(false)
  , m_shGLRenderer(shGLRenderer)
  , m_shGLScene(shGLScene)

{
  m_samples = qglContext->format().samples();
  // Force to track mouse movment when not clicking
  setMouseTracking(true);
  this->setAcceptDrops(true);
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  m_viewport = shGLRenderer->getOrAddViewport(m_viewportIndex);
}

void RTRGLViewportWidget::detachFromRTRViewport() {
  if(m_viewportIndex > -1) m_shGLRenderer->removeViewport(m_viewportIndex);
  m_viewportIndex = -1;
}

RTRGLViewportWidget::~RTRGLViewportWidget() {
  detachFromRTRViewport();
  emit viewportDestroying();
}
 
void RTRGLViewportWidget::setOrthographic(bool orthographic) {
  m_orthographic = orthographic;
  m_shGLRenderer->setOrthographicViewport(m_viewportIndex, m_orthographic);
}

void RTRGLViewportWidget::paintGL() {
  ViewportWidget::computeFPS();
  m_shGLRenderer->render(m_viewportIndex, m_width, m_height, m_samples);
  if(m_alwaysRefresh) update();
}

void RTRGLViewportWidget::resizeGL(int width, int height) {
  m_width = width;
  m_height = height;
}

void RTRGLViewportWidget::toggleAlwaysRefresh() {
  m_alwaysRefresh = !m_alwaysRefresh;
}

FabricCore::RTVal RTRGLViewportWidget::getCamera() {
  return m_shGLRenderer->getCamera(m_viewportIndex);
}


void RTRGLViewportWidget::enterEvent(QEvent * event) {
  grabKeyboard();
}

void RTRGLViewportWidget::leaveEvent(QEvent * event) {
  releaseKeyboard(); 
}

void RTRGLViewportWidget::mousePressEvent(QMouseEvent *event) {
  if(m_shGLScene)
  {
    if(!onEvent(event) && event->button() == Qt::RightButton) 
    {
      SHEditorWidget *contextualMenu = 
        new SHEditorWidget(
          this, 
          m_shGLScene, 
          mapToGlobal(event->pos()));
      emit sceneChanged();
    }
  }
}
 
bool RTRGLViewportWidget::onEvent(QEvent *event) {
  bool redrawAllViewports;
  if(m_shGLRenderer->onEvent(m_viewportIndex, event, redrawAllViewports, false))
  {
    emit manipsAcceptedEvent(redrawAllViewports);
    return true;
  }
  return false;
}

void RTRGLViewportWidget::dragEnterEvent(QDragEnterEvent *event) {
  if(event->mimeData()->hasUrls() && (event->possibleActions() & Qt::CopyAction))
  {
    event->setDropAction( Qt::CopyAction );
    event->accept();
  }
}

void RTRGLViewportWidget::dragMoveEvent(QDragMoveEvent* event) {
  if(event->mimeData()->hasUrls() && (event->possibleActions() & Qt::CopyAction))
  {
    // Convert to a mouseMove event
    QMouseEvent mouseEvent( QEvent::MouseMove, event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier );
    bool redrawAllViewports;
    if( m_shGLRenderer->onEvent( m_viewportIndex, &mouseEvent, redrawAllViewports, true ) )
      emit manipsAcceptedEvent( redrawAllViewports );
  }
}

void RTRGLViewportWidget::dropEvent(QDropEvent *event) {

  if(m_shGLScene)
  {
    const QMimeData *myData = qobject_cast<const QMimeData*>(event->mimeData());
    if(!myData) return;
    if(!event->mimeData()->hasUrls()) return;

    bool forceExpand = event->keyboardModifiers() & Qt::ControlModifier;
    QStringList pathList;
    foreach(QUrl url, event->mimeData()->urls())
      pathList.append(url.toLocalFile());
        
    if(pathList.size() == 0) return;
     
  float pos3D[3];
  m_shGLRenderer->get3DScenePosFrom2DScreenPos( m_viewportIndex, event->pos(), pos3D );
    SHEditorWidget::AddExternalFileList(m_shGLScene, pathList, pos3D, forceExpand);
    event->acceptProposedAction();
    emit sceneChanged();
  }
}
