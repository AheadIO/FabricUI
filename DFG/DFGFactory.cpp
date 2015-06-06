// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGFactory.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGFactory::DFGFactory(const DFGConfig & config)
{
  m_config = config;
}

GraphView::Node * DFGFactory::constructNodeFromPreset(GraphView::Graph * graph, char const * path, char const * preset)
{
  FTL::StrRef presetRef(preset);
  FTL::StrRef lastBit = presetRef.rsplit('.').second;
  return constructNode(graph, path, lastBit.data());
}
