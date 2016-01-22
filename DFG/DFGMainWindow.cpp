// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QDockWidget>
#include <QtGui/QDragEnterEvent>
#include <QtCore/QUrl>

#include "DFGMainWindow.h"

using namespace FabricUI::DFG;

DFGMainWindow::DFGMainWindow(QWidget *parent)
: QMainWindow(parent)
{
}

DFGMainWindow::~DFGMainWindow()
{
}

QDockWidget * DFGMainWindow::addDockWidgetFromWidget(Qt::DockWidgetArea area, QString title, QWidget * widget)
{
  QDockWidget * dock = new QDockWidget(title, this);
  dock->setWidget(widget);
  addDockWidget(area, dock);
  return dock;
}

QWidget * DFGMainWindow::getWidgetFromDockWidget(QString title)
{
  QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();
  for(int i=0;i<dockWidgets.count();i++)
  {
    if(dockWidgets[i]->windowTitle() == title)
      return dockWidgets[i]->widget();
  }
  return NULL;
}
