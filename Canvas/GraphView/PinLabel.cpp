// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/Canvas/GraphView/PinLabel.h>
#include <FabricUI/Canvas/GraphView/Pin.h>
#include <FabricUI/Canvas/GraphView/Graph.h>
#include <FabricUI/Canvas/GraphView/GraphConfig.h>

using namespace FabricUI::GraphView;

PinLabel::PinLabel(Pin * parent, QString text, QColor color, QColor highlightColor, QFont font)
: TextContainer(parent, text, color, highlightColor, font)
{
}
