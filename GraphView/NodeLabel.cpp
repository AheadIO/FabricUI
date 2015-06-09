// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NodeLabel.h"
#include "NodeHeader.h"
#include "Pin.h"
#include "Graph.h"
#include "GraphConfig.h"

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
