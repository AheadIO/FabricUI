// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGGraph__
#define __UI_DFG_DFGGraph__

#include <QtGui/QMenu>

#include <GraphView/Graph.h>

namespace FabricUI
{

  namespace DFG
  {

    class DFGGraph : public GraphView::Graph
    {
      Q_OBJECT

      friend class DFGView;

    public:

      DFGGraph(QGraphicsItem * parent, const GraphView::GraphConfig & config = GraphView::GraphConfig(), GraphView::GraphFactory * factory = NULL);

    };

  };

};

#endif // __UI_DFG_DFGGraph__
