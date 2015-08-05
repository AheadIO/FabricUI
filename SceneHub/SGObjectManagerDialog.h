// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_SceneHub_SGObjectManagerDialog__
#define __UI_SceneHub_SGObjectManagerDialog__

#include <QtCore/QPoint>
#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtGui/QAction>
#include <QtGui/QFrame>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <FabricCore.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SGObjectManagerDialog : public QWidget
    {
      Q_OBJECT

      public:
        SGObjectManagerDialog();
        virtual ~SGObjectManagerDialog() {};
 
      protected:
        bool init(QWidget*, FabricCore::Client *, FabricCore::RTVal);
        virtual void mouseMoveEvent( QMouseEvent * );
        virtual void mouseReleaseEvent( QMouseEvent * );
        virtual void closeEvent( QCloseEvent * );
        void showColorDialog();

        FabricCore::RTVal m_color;
        FabricCore::Client *m_client;
        FabricCore::RTVal m_testObject;
        FabricCore::RTVal m_viewport;
        QWidget* m_parent;
    };
  };
};

#endif // __UI_SceneHub_SGObjectManagerDialog__
