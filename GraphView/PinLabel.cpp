// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/PinLabel.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>

using namespace FabricUI::GraphView;

PinLabel::PinLabel(Pin * parent, QString text, QColor color, QColor highlightColor, QFont font)
: TextContainer(parent, text, color, highlightColor, font)
{
}
