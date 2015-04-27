// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGGraph.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

QSettings * DFGGraph::g_settings = NULL;

DFGGraph::DFGGraph(QGraphicsItem * parent, const GraphView::GraphConfig & config, GraphView::GraphFactory * factory)
: GraphView::Graph(parent, config, factory)
{
}

QSettings * DFGGraph::getSettings()
{
  return g_settings;
}

void DFGGraph::setSettings(QSettings * settings)
{
  g_settings = settings;
}
