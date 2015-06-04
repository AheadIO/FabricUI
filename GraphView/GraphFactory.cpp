// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "GraphFactory.h"
#include "Graph.h"
#include "Node.h"

using namespace FabricUI::GraphView;

Node * GraphFactory::constructNode(Graph * graph, char const *name, char const * label)
{
  return new Node(graph, name, label);
}

Connection * GraphFactory::constructConnection(Graph * graph, ConnectionTarget * src, ConnectionTarget * dst)
{
  return new Connection(graph, src, dst);
}
