// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGGraph__
#define __UI_DFG_DFGGraph__

#include <QtGui/QMenu>
#include <QtCore/QSettings>

#include <FabricUI/GraphView/Graph.h>

namespace FabricUI
{

  namespace DFG
  {

    class DFGGraph : public GraphView::Graph
    {
      Q_OBJECT

      friend class DFGNotificationRouter;

    public:

      DFGGraph(QGraphicsItem * parent, const GraphView::GraphConfig & config = GraphView::GraphConfig(), GraphView::GraphFactory * factory = NULL);

      static QSettings * getSettings();
      static void setSettings(QSettings * settings);

    private:
      static QSettings * g_settings;

    };

  };

};

#endif // __UI_DFG_DFGGraph__
