// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/Canvas/GraphView/SidePanelItemGroup.h>

using namespace FabricUI::GraphView;

SidePanelItemGroup::SidePanelItemGroup(QGraphicsWidget * parent)
: QGraphicsWidget(parent)
{
}

void SidePanelItemGroup::resizeEvent(QGraphicsSceneResizeEvent * event)
{
  QGraphicsWidget::resizeEvent(event);
  emit resized();
}
