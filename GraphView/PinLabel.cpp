// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "PinLabel.h"
#include "Pin.h"
#include "Graph.h"
#include "GraphConfig.h"

using namespace FabricUI::GraphView;

PinLabel::PinLabel(Pin * parent, QString text, QColor color, QColor highlightColor, QFont font)
: TextContainer(parent, text, color, highlightColor, font)
{
}
