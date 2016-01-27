// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGMainWindow__
#define __UI_DFG_DFGMainWindow__

#include <QtGui/QMainWindow>

namespace FabricUI
{

  namespace DFG
  {

    class DFGMainWindow : public QMainWindow
    {
      Q_OBJECT

    public:

      DFGMainWindow(QWidget * parent = 0);
      virtual ~DFGMainWindow();

      QDockWidget * addDockWidgetFromWidget(Qt::DockWidgetArea area, QString title, QWidget * widget);
      QWidget * getWidgetFromDockWidget(QString title);
    };

  };

};

#endif // __UI_DFG_DFGMainWindow__
