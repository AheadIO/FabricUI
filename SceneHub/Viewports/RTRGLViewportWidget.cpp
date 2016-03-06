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
  SHGLScene *shGLScene,
  SHGLRenderer *shGLRenderer,
  int viewportIndex, 
  QGLContext *qglContext, 
  QWidget *parent, 
  QGLWidget *share,
  QSettings *settings) 
  : ViewportWidget(&shGLScene->getClient(), QColor(), qglContext, parent, share, settings)
  , m_viewportIndex(viewportIndex)
  , m_alwaysRefresh(false)
  , m_orthographic(false)
  , m_shGLScene(shGLScene)
  , m_shGLRenderer(shGLRenderer)
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
  if(!onEvent(event) && event->button() == Qt::RightButton) 
  {
    SHEditorWidget *contextualMenu = 
      new SHEditorWidget(
        this, 
        mapToGlobal(event->pos()), 
        m_shGLScene,
        m_shGLRenderer);
    emit sceneChanged();
  }
}
 
bool RTRGLViewportWidget::onEvent(QEvent *event) {
  bool redrawAllViewports;
  if(m_shGLRenderer->onEvent(m_viewportIndex, event, redrawAllViewports))
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
    // Simulate a MousePressEvent to automatically select the scene object
    // Use to drop a texture on a mesh, or add a asset as child of a scene instance
    QMouseEvent mouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton,  Qt::NoModifier);
    mousePressEvent(&mouseEvent);
    event->acceptProposedAction();
  }
}

void RTRGLViewportWidget::dropEvent(QDropEvent *event) {
  const QMimeData *myData = qobject_cast<const QMimeData*>(event->mimeData());
  if(!myData) return;
  if(!event->mimeData()->hasUrls()) return;

  bool forceExpand = event->keyboardModifiers() & Qt::ControlModifier;
  QStringList pathList;
  foreach(QUrl url, event->mimeData()->urls())
    pathList.append(url.toLocalFile());
      
  if(pathList.size() == 0) return;
   
  float pos[2], pos3D[3];
  pos[0] = event->pos().x();
  pos[1] = event->pos().y();
  m_shGLRenderer->get3DScenePosFrom2DScreenPos(m_viewportIndex, pos, pos3D);
  SHEditorWidget::AddExternalFileList(m_shGLScene, pathList, pos3D, forceExpand);
  event->acceptProposedAction();
  emit sceneChanged();
}
