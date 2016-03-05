/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGBASEMANAGERDIALOG_H__
#define __UI_SCENEHUB_SGBASEMANAGERDIALOG_H__

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
    class SGBaseManagerDialog : public QWidget
    {
      Q_OBJECT

      public:
        SGBaseManagerDialog();
        virtual ~SGBaseManagerDialog() {};
 
      protected:
        bool init(QWidget*, FabricCore::Client, FabricCore::RTVal);
        virtual void mouseMoveEvent( QMouseEvent * );
        virtual void mouseReleaseEvent( QMouseEvent * );
        virtual void closeEvent( QCloseEvent * );
        bool showColorDialog();

        FabricCore::RTVal m_color;
        FabricCore::Client m_client;
        FabricCore::RTVal m_testObject;
        QWidget* m_parent;
    };
  };
};

#endif // __UI_SCENEHUB_SGBASEMANAGERDIALOG_H__
