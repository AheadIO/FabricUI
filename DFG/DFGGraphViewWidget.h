// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGGraphViewWidget__
#define __UI_DFG_DFGGraphViewWidget__

#include <GraphView/GraphViewWidget.h>

namespace FabricUI
{
  namespace DFG
  {

    class DFGGraphViewWidget : public GraphView::GraphViewWidget
    {

    public:

      DFGGraphViewWidget(QWidget * parent, const GraphView::GraphConfig & config = GraphView::GraphConfig(), GraphView::GraphFactory * factory = NULL, GraphView::Graph * graph = NULL);

      virtual void dropEvent(QDropEvent *event);

    };

  };

};

#endif // __UI_DFG_DFGGraphViewWidget__
