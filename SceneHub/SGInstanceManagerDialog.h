// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_SceneHub_SGInstanceManagerDialog__
#define __UI_SceneHub_SGInstanceManagerDialog__

#include <QtCore/QPoint>
#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtGui/QAction>
#include <QtGui/QFrame>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include "SGObjectManagerDialog.h"
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SGInstanceManagerDialog : public SGObjectManagerDialog
    {
      Q_OBJECT

      public:
        SGInstanceManagerDialog(QWidget* parent, FabricCore::Client *client, FabricCore::RTVal testObject);
        virtual ~SGInstanceManagerDialog() {};
        
        void setInstanceProperties();
        void addInstance(QPoint screenPos);

      public slots:
        void showColorDialog();
        void updatetInstanceProperties();

      private:
        FabricCore::RTVal showFileDialog();
        
        bool m_addInstance;
        QPoint m_glWidgetScreenPos;
        QPushButton *m_colorButton;
        QPushButton *m_addButton;
    };
  };
};

#endif // __UI_SceneHub_SGInstanceManagerDialog__
