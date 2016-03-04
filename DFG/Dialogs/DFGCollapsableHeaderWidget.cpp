// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "DFGCollapsableHeaderWidget.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGCollapsableHeaderWidget::DFGCollapsableHeaderWidget(QWidget * parent, QString title, const DFGConfig & dfgConfig)
: QLabel(title, parent)
, m_dfgConfig(dfgConfig)
{
  QPalette pal(palette());
 
  pal.setColor(QPalette::Window, pal.color(QPalette::Window).darker());
  pal.setColor(QPalette::WindowText, pal.color(QPalette::WindowText).darker());
  setAutoFillBackground(true);
  setPalette(pal);
}

DFGCollapsableHeaderWidget::~DFGCollapsableHeaderWidget()
{
}

void DFGCollapsableHeaderWidget::mousePressEvent(QMouseEvent * event)
{
  emit toggled();
  event->accept();
}
