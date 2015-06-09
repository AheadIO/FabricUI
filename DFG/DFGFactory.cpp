// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGFactory.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGFactory::DFGFactory(const DFGConfig & config)
{
  m_config = config;
}

GraphView::Node * DFGFactory::constructNodeFromPreset(
  GraphView::Graph * graph,
  FTL::StrRef path,
  FTL::StrRef preset
  )
{
  FTL::StrRef lastBit = preset.rsplit('.').second;
  return constructNode(graph, path, lastBit);
}
