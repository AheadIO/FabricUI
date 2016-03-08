/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_EDITOR_WIDGET_H__
#define __UI_SCENEHUB_EDITOR_WIDGET_H__

#include <QtGui/QMenu>
#include <QtGui/QDialog>
#include <FabricUI/SceneHub/SHGLScene.h>
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SHEditorWidget : public QMenu {

      Q_OBJECT

      public:

        static void AddExternalFileList(
          SHGLScene *shGLScene,
          QStringList pathList, 
          float pos3D[3], 
          bool forceExpand); 

        static void AddExternalFile(
          SHGLScene *shGLScene,
          QString path, 
          float pos3D[3], 
          bool forceExpand);

        static void ImportAsset(SHGLScene *shGLScene);

        static void ExportToAlembic(SHGLScene *shGLScene);


        SHEditorWidget(QWidget* parent, SHGLScene *shGLScene, const QPoint &point);

        ~SHEditorWidget() {} 


      public slots:
        void addLight();

        void addArchive();

        void addTexture();
       
        void editObjectColor() { editObjectColor(false); }

        void editLocalObjectColor() { editObjectColor(true); }

        void editLightProperties();


      protected:
        void editObjectColor(bool local);

        void constuctLightMenu();

        void constuctGeometryMenu(QString category);

        void constuctAddMenu();

        SHGLScene *m_shGLScene;
    };
  }
}


#endif // __UI_SCENEHUB_EDITOR_WIDGET_H__
