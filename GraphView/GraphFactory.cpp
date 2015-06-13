// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "GraphFactory.h"
#include "Graph.h"
#include "Node.h"

using namespace FabricUI::GraphView;

Node * GraphFactory::constructNode(
  Graph * graph,
  FTL::CStrRef name,
  FTL::CStrRef label
  )
{
  return new Node(graph, name, label);
}
