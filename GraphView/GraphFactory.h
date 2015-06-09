// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphFactory__
#define __UI_GraphView_GraphFactory__

#include "Node.h"
#include "Connection.h"

#include <QtCore/QString>
#include <QtGui/QColor>

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;

    class GraphFactory
    {
      friend class Graph;

    public:

      GraphFactory() {}
      virtual ~GraphFactory() {}

    protected:

      virtual Node * constructNode(
        Graph * graph,
        FTL::StrRef name,
        FTL::StrRef label = FTL::StrRef()
        );

      virtual Node * constructNodeFromPreset(
        Graph * graph,
        FTL::StrRef name,
        FTL::StrRef preset
        ) = 0;

      virtual Connection * constructConnection(Graph * graph, ConnectionTarget * src, ConnectionTarget * dst);
    };

  };

};

#endif // __UI_GraphView_GraphFactory__
