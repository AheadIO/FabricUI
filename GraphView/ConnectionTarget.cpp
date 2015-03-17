// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ConnectionTarget.h"
#include "Graph.h"

using namespace FabricUI::GraphView;

bool ConnectionTarget::isConnected() const
{
  return graph()->isConnected(this);
}
