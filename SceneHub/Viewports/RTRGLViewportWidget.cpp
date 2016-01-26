/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <FTL/Config.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <memory>
#include <assert.h>
#if defined(FTL_OS_DARWIN)
# include <CoreFoundation/CFURL.h>
#endif

#include <QtGui/qapplication.h>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFileDialog>
#include <QtGui/QColorDialog>
#include <FabricUI/SceneHub/Viewports/RTRGLViewportWidget.h>
#include <FabricUI/Util/QtToKLEvent.h>
#include <FabricUI/Util/macros.h>

using namespace FabricUI::Viewports;


RTRGLViewportWidget::RTRGLViewportWidget(
  FabricCore::Client *client,
  FabricCore::RTVal shObject, 
  int viewportIndex, 
  QGLContext *qglContext, 
  QWidget *parent, 
  QGLWidget *share,
  QSettings *settings) 
  : ViewportWidget(client, QColor(), qglContext, parent, share, settings)
  , m_viewportIndex(viewportIndex)
  , m_alwaysRefresh(false)
  , m_shObject(shObject)
{
  m_geometryDialog = new FabricUI::SceneHub::SGGeometryManagerDialog(this, m_client, m_shObject);
  m_lightDialog = new FabricUI::SceneHub::SGLightManagerDialog(this, m_client, m_shObject);
  m_viewportIndexRTVal = FabricCore::RTVal::ConstructUInt32( *m_client, viewportIndex );
  m_samples = FabricCore::RTVal::ConstructUInt32( *m_client, qglContext->format().samples() );

  // Force to track mouse movment when not clicking
  setMouseTracking(true);
  this->setAcceptDrops(true);
  this->setContextMenuPolicy(Qt::CustomContextMenu);
 
  FABRIC_TRY("RTRGLViewportWidget::RTRGLViewportWidget create viewport",
    m_viewport = m_shObject.callMethod("Viewport_Virtual", "getOrAddViewport", 1, &m_viewportIndexRTVal); 
  );
}

void RTRGLViewportWidget::detachFromRTRViewport() {
  FABRIC_TRY( "RTRGLViewportWidget::~RTRGLViewportWidget remove viewport",
              if( m_shObject.isValid() && m_viewportIndexRTVal.isValid() )
                m_shObject.callMethod( "", "removeViewport", 1, &m_viewportIndexRTVal );
  );
  m_viewportIndexRTVal = FabricCore::RTVal();
}

RTRGLViewportWidget::~RTRGLViewportWidget() {
  detachFromRTRViewport();
  emit viewportDestroying();
}
 
void RTRGLViewportWidget::paintGL() {
  ViewportWidget::computeFPS();
  
  FABRIC_TRY("RTRGLViewportWidget::paintGL", 
    FabricCore::RTVal args[4];
    args[0] = m_viewportIndexRTVal;
    args[1] = m_width;
    args[2] = m_height;
    args[3] = m_samples;
    m_shObject.callMethod("", "render", 4, args);
  );

  if(m_alwaysRefresh) update();
}

void RTRGLViewportWidget::resizeGL(int width, int height) {
  FABRIC_TRY("RTRGLViewportWidget::resizeGL",
    m_width = FabricCore::RTVal::ConstructUInt32(*m_client, width);
    m_height = FabricCore::RTVal::ConstructUInt32(*m_client, height);
  );
}

void RTRGLViewportWidget::toggleAlwaysRefresh() {
  m_alwaysRefresh = !m_alwaysRefresh;
}

FabricCore::RTVal RTRGLViewportWidget::getCamera() {
  return m_viewport.callMethod("Camera", "getCamera", 0, 0);
}


//*************

void RTRGLViewportWidget::addExternalFile(QStringList paths, QPoint mousePos, bool forceExpand) {

  std::vector<std::string> pathList;
  for(int i=0; i<paths.size(); ++i) 
  {
    QString localFileQString = paths[i];
#if defined(FTL_OS_DARWIN)
      // [pzion 20150805] Work around
      // https://bugreports.qt.io/browse/QTBUG-40449
      if ( localFileQString.startsWith("/.file/id=") )
      {
        CFStringRef relCFStringRef =
          CFStringCreateWithCString(
            kCFAllocatorDefault,
            localFileQString.toUtf8().constData(),
            kCFStringEncodingUTF8
            );
        CFURLRef relCFURL =
          CFURLCreateWithFileSystemPath(
            kCFAllocatorDefault,
            relCFStringRef,
            kCFURLPOSIXPathStyle,
            false // isDirectory
            );
        CFErrorRef error = 0;
        CFURLRef absCFURL =
          CFURLCreateFilePathURL(
            kCFAllocatorDefault,
            relCFURL,
            &error
            );
        if ( !error )
        {
          static const CFIndex maxAbsPathCStrBufLen = 4096;
          char absPathCStr[maxAbsPathCStrBufLen];
          if ( CFURLGetFileSystemRepresentation(
            absCFURL,
            true, // resolveAgainstBase
            reinterpret_cast<UInt8 *>( &absPathCStr[0] ),
            maxAbsPathCStrBufLen
            ) )
          {
            localFileQString = QString( absPathCStr );
          }
        }
        CFRelease( absCFURL );
        CFRelease( relCFURL );
        CFRelease( relCFStringRef );
      }
#endif
    pathList.push_back( localFileQString.toStdString() );
  }

  if(pathList.size() > 0)
  {
    FABRIC_TRY("RTRGLViewportWidget::addExternalFile",
      FabricCore::RTVal klPathList = FabricCore::RTVal::ConstructVariableArray(*m_client, "String");
      klPathList.setArraySize(pathList.size());
      for(uint8_t i=0; i<pathList.size(); ++i) 
        klPathList.setArrayElement(i, FabricCore::RTVal::ConstructString(*m_client, pathList[i].c_str()));
      
      std::vector<FabricCore::RTVal> klParams;
      klParams.push_back(m_viewportIndexRTVal);
      klParams.push_back(klPathList);
      klParams.push_back(QtToKLMousePosition(mousePos, *m_client, m_viewport, true));
      klParams.push_back(FabricCore::RTVal::ConstructBoolean(*m_client, forceExpand));

      m_shObject.callMethod("", "onAddExternalFile", 4, &klParams[0]);
      emit sceneChanged();
    );
  }
}
 
void RTRGLViewportWidget::onContextMenu(const QPoint &point) {

  std::string category = "";
  FABRIC_TRY("RTRGLViewportWidget::onContextMenu",
    FabricCore::RTVal klCategory = m_shObject.callMethod("String", "getSelectionCategory", 0, 0);
    category = klCategory.getStringCString();
   );

  if(point.x() > 0) 
  {
    m_screenPos = point;
    if(category.compare("") == 0) constuctAddMenu();
    else if(category.compare("geometry") == 0 || category.compare("parent") == 0) constuctGeometryMenu(category);
    else if(category.compare("light") == 0) constuctLightMenu();
  }
  else
  {
    if(m_geometryDialog->isVisible() || m_lightDialog->isVisible())
    {
      if(category.compare("geometry") == 0 || category.compare("parent") == 0) constuctGeometryMenu(category);
      else if (category.compare("light") == 0) editLightProperties();
    }
  }
}

void RTRGLViewportWidget::constuctAddMenu() {
  QMenu menu;
  QAction* addArchiveAction = menu.addAction("Add Geometry");
  QAction* addArchiveExpandAction = menu.addAction("Add Geometry (Force Expand");
  menu.addSeparator();
  QAction* addSpotLightAction = menu.addAction("Add Spot");
  QAction* addPointLightAction = menu.addAction("Add Point");
  QAction* addDirectLightAction = menu.addAction("Add Directional");
  QAction* addShadowSpotLightAction = menu.addAction("Add Shadow Spot");
  QAction* addShadowPointLightAction = menu.addAction("Add Shadow Point");
  QAction* addShadowDirectLightAction = menu.addAction("Add Shadow Directional");
 
  QObject::connect(addArchiveAction, SIGNAL( triggered() ),this, SLOT( addArchive() ) );
  QObject::connect(addArchiveExpandAction, SIGNAL( triggered() ),this, SLOT( addArchive() ) );
  QObject::connect(addSpotLightAction, SIGNAL( triggered() ),this, SLOT( addLight() ) );
  QObject::connect(addPointLightAction, SIGNAL( triggered() ),this, SLOT( addLight() ) );
  QObject::connect(addDirectLightAction, SIGNAL( triggered() ),this, SLOT( addLight() ) );
  QObject::connect(addShadowSpotLightAction, SIGNAL( triggered() ),this, SLOT( addLight() ) );
  QObject::connect(addShadowPointLightAction, SIGNAL( triggered() ),this, SLOT( addLight() ) );
  QObject::connect(addShadowDirectLightAction, SIGNAL( triggered() ),this, SLOT( addLight() ) );
  menu.exec(this->mapToGlobal(m_screenPos));
}

void RTRGLViewportWidget::addLight() {
  
  // Get the light we want to add
  uint32_t lightType = 0;
  std::string lightName = dynamic_cast<QAction*>( QObject::sender())->text().toStdString();
  if(lightName.compare("Add Point") == 0) lightType = 0;
  else if(lightName.compare("Add Spot") == 0) lightType = 1;
  else if(lightName.compare("Add Directional") == 0) lightType = 2;
  else if(lightName.compare("Add Shadow Point") == 0) lightType = 3;
  else if(lightName.compare("Add Shadow Spot") == 0) lightType = 4;
  else if(lightName.compare("Add Shadow Directional") == 0) lightType = 5;

  FABRIC_TRY("RTRGLViewportWidget::setLightProperties",
    std::vector<FabricCore::RTVal> klParams(3); 
    klParams[0] = m_viewportIndexRTVal;
    klParams[1] = FabricCore::RTVal::ConstructUInt32(*m_client, lightType);
    klParams[2] = QtToKLMousePosition(m_screenPos, *m_client, m_viewport, true);
    m_shObject.callMethod("", "onAddLight", 3, &klParams[0]); 
    emit sceneChanged();
  );
}

void RTRGLViewportWidget::addArchive() {

  bool forceExpand = false;
  std::string name = dynamic_cast<QAction*>( QObject::sender())->text().toStdString();
  if(name.compare("Add Geometry") != 0) forceExpand = true;
 
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setNameFilter(tr("Files (*.abc *.fbx)"));
  QStringList fileNames;
  if (dialog.exec())
    fileNames = dialog.selectedFiles();
 
  addExternalFile(fileNames, m_screenPos, forceExpand);
}

void RTRGLViewportWidget::addTexture() {

  // bool forceExpand = false;
  std::string name = dynamic_cast<QAction*>( QObject::sender())->text().toStdString();
  
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFiles);
  QStringList fileNames;
  if (dialog.exec())
    fileNames = dialog.selectedFiles();
 
  addExternalFile(fileNames, m_screenPos, false);
}

void RTRGLViewportWidget::constuctGeometryMenu(std::string category) {
  if(m_lightDialog->isVisible())
    m_lightDialog->close();

  // Directly call the color box
  if(category.compare("parent") == 0)
    editObjectColor();

  // However, a geometry can either has color ortexture
  else if(category.compare("geometry") == 0)
  {
    QMenu menu;
    QAction* editColor = menu.addAction("Edit Color");
    QObject::connect(editColor, SIGNAL( triggered() ),this, SLOT( editObjectColor() ) );

    QAction* editLocalColor = menu.addAction("Edit local Color");
    QObject::connect(editLocalColor, SIGNAL( triggered() ),this, SLOT( editLocalObjectColor() ) );

    QAction* setTextureAction = menu.addAction("Set Texture");
    QObject::connect(setTextureAction, SIGNAL( triggered() ),this, SLOT( addTexture() ) );
    menu.exec(this->mapToGlobal(m_screenPos));
  }
}

void RTRGLViewportWidget::editObjectColor( bool local ) {
  
  QColor color = QColorDialog::getColor(Qt::yellow, this);
  if(color != QColor::Invalid)
  {
    FABRIC_TRY("SGBaseManagerDialog::showColorDialog",
      std::vector<FabricCore::RTVal> klColorRGBA(4); 
      klColorRGBA[0] = FabricCore::RTVal::ConstructFloat32(*m_client, float(color.redF()));
      klColorRGBA[1] = FabricCore::RTVal::ConstructFloat32(*m_client, float(color.greenF()));
      klColorRGBA[2] = FabricCore::RTVal::ConstructFloat32(*m_client, float(color.blueF()));
      klColorRGBA[3] = FabricCore::RTVal::ConstructFloat32(*m_client, 1.0f);
      FabricCore::RTVal args[2];
      args[0] = FabricCore::RTVal::Construct(*m_client, "Color", 4, &klColorRGBA[0]);
      args[1] = FabricCore::RTVal::ConstructBoolean(*m_client, local);
      m_shObject.callMethod("", "onSetObjectColor", 2, args); 
      emit sceneChanged();
    );
  }
}

void RTRGLViewportWidget::editObjectColor() {
  editObjectColor(false);
}

void RTRGLViewportWidget::editLocalObjectColor() {
  editObjectColor(true);
}

void RTRGLViewportWidget::constuctLightMenu() {
  QMenu menu;
  menu.setTitle("Light");
  QAction* propertiesAction = menu.addAction("Edit Properties");
  QObject::connect(propertiesAction, SIGNAL( triggered() ),this, SLOT( editLightProperties() ) );
  menu.exec(this->mapToGlobal(m_screenPos));
}

void RTRGLViewportWidget::editLightProperties() {
  //if(m_geometryDialog->isVisible())
  //  m_geometryDialog->close();
  int w = int(0.2*this->width());
  w = (w < 50) ? 50 : w;
  int h = int(0.2*this->height());
  h = (h < 50) ? 50 : h;
  m_lightDialog->setFixedSize(w, h);
  m_lightDialog->show();
}


//*************

void RTRGLViewportWidget::mousePressEvent(QMouseEvent *event) {
  if(!onEvent(event) && event->button() == Qt::RightButton) 
  {
    const QPoint pos = event->pos();
    onContextMenu(pos);
  }
}

void RTRGLViewportWidget::mouseMoveEvent(QMouseEvent *event) {
  onEvent(event);
}

void RTRGLViewportWidget::wheelEvent(QWheelEvent *event) {
  onEvent(event);
}

void RTRGLViewportWidget::mouseReleaseEvent(QMouseEvent *event) {
  onEvent(event);
  emit synchronizeCommands(false);
}

void RTRGLViewportWidget::keyPressEvent(QKeyEvent *event) {
  onEvent(event);
}

void RTRGLViewportWidget::keyReleaseEvent(QKeyEvent *event) {
  onEvent(event);
}

bool RTRGLViewportWidget::onEvent(QEvent *event) {
  bool result = false;
  FABRIC_TRY_RETURN("RTRGLViewportWidget::onEvent", false,
    
    FabricCore::RTVal klevent = QtToKLEvent(event, *m_client, m_viewport, true);
    m_shObject.callMethod("", "onEvent", 1, &klevent);
    
    result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
    event->setAccepted(result);
    
    bool redrawAllViewports = klevent.callMethod("Boolean", "redrawAllViewports", 0, 0).getBoolean();
    if(result) emit manipsAcceptedEvent( redrawAllViewports );
    return result;
  );
}


//*************

void RTRGLViewportWidget::dragEnterEvent(QDragEnterEvent *event) {
  if(event->mimeData()->hasUrls() && (event->possibleActions() & Qt::CopyAction))
  {
    event->setDropAction( Qt::CopyAction );
    event->accept();
  }
}

void RTRGLViewportWidget::dragMoveEvent( QDragMoveEvent* event ) {
  if ( event->mimeData()->hasUrls() && ( event->possibleActions() & Qt::CopyAction ) )
  {
    // Simulate a MousePressEvent to automatically select the scene object
    // Use to drop a texture on a mesh, or add a asset as child of a scene instance
    QMouseEvent mouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton,  Qt::NoModifier);
    mousePressEvent(&mouseEvent);
    event->acceptProposedAction();
  }
}

void RTRGLViewportWidget::dropEvent(QDropEvent *event) {
  const QMimeData *myData = qobject_cast<const QMimeData *>(event->mimeData());
  if(!myData) return;
  if(!event->mimeData()->hasUrls()) return;
  bool forceExpand = event->keyboardModifiers() & Qt::ControlModifier;
  QStringList paths;
  foreach (QUrl url, event->mimeData()->urls())
    paths.append(url.toLocalFile());
  addExternalFile(paths, event->pos(), forceExpand);
  event->acceptProposedAction();
}
