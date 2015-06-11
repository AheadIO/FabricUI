// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/NodeLabel.h>
#include <FabricUI/GraphView/NodeHeader.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>

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
