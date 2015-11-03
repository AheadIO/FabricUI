// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_SceneHub_SGGeometryManagerDialog__
#define __UI_SceneHub_SGGeometryManagerDialog__

#include <QtCore/QPoint>
#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtGui/QAction>
#include <QtGui/QFrame>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <FabricUI/SceneHub/Managers/SGBaseManagerDialog.h>
  
namespace FabricUI
{
  namespace SceneHub
  {
    class SGGeometryManagerDialog : public SGBaseManagerDialog
    {
      Q_OBJECT

      public:
        SGGeometryManagerDialog(QWidget* parent, FabricCore::Client *client, FabricCore::RTVal testObject);
        virtual ~SGGeometryManagerDialog() {};
 
      public slots:
        void showColorDialog();
        void updatetGeometryProperties();

      private:       
        QPoint m_glWidgetScreenPos;
        QPushButton *m_colorButton;
        QPushButton *m_closeButton;

    };
  };
};

#endif // __UI_SceneHub_SGGeometryManagerDialog__
