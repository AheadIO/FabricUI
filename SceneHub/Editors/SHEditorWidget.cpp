/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHEditorWidget.h"
#include "SHLightEditorDialog.h"
#include "SHGeometryEditorDialog.h"
#include <FabricUI/Viewports/QtToKLEvent.h>
#include <QtGui/QFileDialog>
#include <QtGui/QColorDialog>
using namespace FabricCore;
using namespace FabricUI::SceneHub;


// *************************
inline QStringList ProcessPathQStringForOsX(QStringList pathList) {
  QStringList pathList_;
  for(int i=0; i<pathList.size(); ++i) 
  {
    QString localFileQString = pathList[i];
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
    pathList_.append(localFileQString);
  }
  return pathList_;
}

void SHEditorWidget::AddExternalFileList(
  SHGLScene *shGLScene,
  QStringList pathList, 
  float pos3D[3], 
  bool forceExpand) 
{ 
  QStringList pathList_ = ProcessPathQStringForOsX(pathList);
  if(pathList_.size() > 0)
  {
    try 
    {
      shGLScene->addExternalFileList(pathList_, pos3D, forceExpand);
    }
    catch(Exception e)
    {
      printf("SHEditorWidget::AddExternalFileList: exception: %s\n", e.getDesc_cstr() );
    }
  }
}

void SHEditorWidget::AddExternalFile(
  SHGLScene *shGLScene,
  QString path, 
  float pos3D[3], 
  bool forceExpand) 
{ 
  QStringList pathList; pathList.append(path);
  AddExternalFileList(shGLScene, pathList, pos3D, forceExpand);
}

void SHEditorWidget::ImportAsset(SHGLScene *shGLScene) { 
 
  QFileDialog dialog(0, "Import Assets", "", "*.abc *.fbx");
  dialog.setFileMode(QFileDialog::ExistingFiles);
  if(dialog.exec())
  {
    QStringList pathList = dialog.selectedFiles();
    if(pathList.size() > 0)
    {
      float pos[3]; pos[0] = pos[1] = pos[2] = 0.0f;
      AddExternalFileList(shGLScene, pathList, pos, false);
    }
  }
}

void SHEditorWidget::ExportToAlembic(SHGLScene *shGLScene) { 

  QString filePath = QFileDialog::getSaveFileName(0, "Export scene to Alembic", "", "*.abc");
  if(filePath.length() == 0)
    return;
  if(filePath.toLower().endsWith(".abs.abc"))
    filePath = filePath.left(filePath.length() - 4);
  else if(!filePath.toLower().endsWith(".abc"))
    filePath += ".abc";

  try 
  {
    QStringList pathList; pathList.append(filePath);
    pathList = ProcessPathQStringForOsX(pathList);
    shGLScene->exportToAlembic(pathList[0]);
  }
  catch(Exception e)
  {
    printf("SHEditorWidget::ExportToAlembic: exception: %s\n", e.getDesc_cstr() );
  }
}


// *************************
SHEditorWidget::SHEditorWidget(
  QWidget* parent,
  const QPoint &point,
  SHGLScene *shGLScene,
  SHGLRenderer *shGLRenderer) 
    : QMenu(parent)
    , m_shGLScene(shGLScene)
    , m_shGLRenderer(shGLRenderer)
{
  QString category = m_shGLRenderer->getSelectionCategory();
 
  if(point.x() > 0) 
  {
    //m_screenPos = point;
    if(category == "") constuctAddMenu();
    else if(category == "geometry" || category == "parent") constuctGeometryMenu(category);
    else if(category == "light") constuctLightMenu();
  }
  else
  {
    if(category == "geometry" || category == "parent") constuctGeometryMenu(category);
    else if (category == "light") editLightProperties();
  }

  exec(point);
}

void SHEditorWidget::constuctAddMenu() {
  QAction* addArchiveAction = addAction("Add Geometry");
  QAction* addArchiveExpandAction = addAction("Add Geometry (Force Expand");
  addSeparator();
  QAction* addSpotLightAction = addAction("Add Spot");
  QAction* addPointLightAction = addAction("Add Point");
  QAction* addDirectLightAction = addAction("Add Directional");
  QAction* addShadowSpotLightAction = addAction("Add Shadow Spot");
  QAction* addShadowPointLightAction = addAction("Add Shadow Point");
  QAction* addShadowDirectLightAction = addAction("Add Shadow Directional");
 
  QObject::connect(addArchiveAction, SIGNAL(triggered()), this, SLOT(addArchive()));
  QObject::connect(addArchiveExpandAction, SIGNAL(triggered()), this, SLOT(addArchive()));
  QObject::connect(addSpotLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
  QObject::connect(addPointLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
  QObject::connect(addDirectLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
  QObject::connect(addShadowSpotLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
  QObject::connect(addShadowPointLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
  QObject::connect(addShadowDirectLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
}

void SHEditorWidget::constuctGeometryMenu(QString category) {
  // Directly call the color box
  if(category == "parent")
    editObjectColor();

  // However, a geometry can either has color ortexture
  else if(category == "geometry")
  {
    QAction* editColor = addAction("Edit Color");
    QObject::connect(editColor, SIGNAL(triggered()), this, SLOT(editObjectColor()));
    QAction* editLocalColor = addAction("Edit local Color");
    QObject::connect(editLocalColor, SIGNAL(triggered()), this, SLOT(editLocalObjectColor()));
    QAction* setTextureAction = addAction("Set Texture");
    QObject::connect(setTextureAction, SIGNAL(triggered()), this, SLOT(addTexture()));
  }
}

void SHEditorWidget::constuctLightMenu() {
  setTitle("Light");
  QAction* propertiesAction = addAction("Edit Properties");
  QObject::connect(propertiesAction, SIGNAL( triggered() ),this, SLOT( editLightProperties() ) );
}

void SHEditorWidget::addLight() {
  
  // Get the light we want to add
  uint32_t lightType = 0;
  QString lightName = dynamic_cast<QAction*>(QObject::sender())->text();
  if(lightName == "Add Point") lightType = 0;
  else if(lightName == "Add Spot") lightType = 1;
  else if(lightName == "Add Directional") lightType = 2;
  else if(lightName == "Add Shadow Point") lightType = 3;
  else if(lightName == "Add Shadow Spot") lightType = 4;
  else if(lightName == "Add Shadow Directional") lightType = 5;
 
  float pos[3]; pos[0] = pos[1] = pos[2] = 0.0f;
  m_shGLScene->addLight(lightType, pos);
}

void SHEditorWidget::addArchive() {

  bool forceExpand = false;
  QString senderName = dynamic_cast<QAction*>( QObject::sender())->text();
  if(senderName == "Add Geometry") forceExpand = true;
 
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setNameFilter(tr("Files (*.abc *.fbx)"));
  QStringList pathList;
  if(dialog.exec())
  {
    pathList = dialog.selectedFiles();
    float pos[3]; pos[0] = pos[1] = pos[2] = 0.0f;
    AddExternalFileList(m_shGLScene, pathList, pos, forceExpand);
  }
}

void SHEditorWidget::addTexture() {

  bool forceExpand = false;
  //std::string name = dynamic_cast<QAction*>( QObject::sender())->text().toStdString();
  
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFiles);
  QStringList pathList;
  if(dialog.exec())
  {
    pathList = dialog.selectedFiles();
    float pos[3]; pos[0] = pos[1] = pos[2] = 0.0f;
    AddExternalFileList(m_shGLScene, pathList, pos, forceExpand);
  }
}

void SHEditorWidget::editObjectColor( bool local ) {
  QColor color = QColorDialog::getColor(Qt::yellow, this);
  if(color != QColor::Invalid)
  {
    float color_[4]; 
    color_[0] = color.redF();
    color_[1] = color.greenF();
    color_[2] = color.blueF();
    color_[3] = color.alphaF();
    m_shGLScene->setObjectColor(color_, local);
  }
}

void SHEditorWidget::editLightProperties() {
  //if(m_geometryDialog->isVisible())
  //  m_geometryDialog->close();
  //int w = int(0.2*this->width());
  //w = (w < 50) ? 50 : w;
  //int h = int(0.2*this->height());
  //h = (h < 50) ? 50 : h;
  //SHLightEditorDialog *lightDialog = new SHLightEditorDialog(
  //  this, 
  //  m_shGLScene);
  //lightDialog->setFixedSize(w, h);
  //lightDialog->show();
}
