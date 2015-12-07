// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/Canvas/GraphView/NodeLabel.h>
#include <FabricUI/Canvas/GraphView/NodeHeader.h>
#include <FabricUI/Canvas/GraphView/Pin.h>
#include <FabricUI/Canvas/GraphView/Graph.h>
#include <FabricUI/Canvas/GraphView/GraphConfig.h>

using namespace FabricUI::GraphView;

NodeLabel::NodeLabel(
  NodeHeader * parent,
  QString const &text,
  QColor color,
  QColor highlightColor,
  QFont font
  )
  : TextContainer(
    parent,
    text,
    color,
    highlightColor,
    font
    )
{
}
