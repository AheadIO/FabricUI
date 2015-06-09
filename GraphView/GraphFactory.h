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
        FTL::CStrRef name,
        FTL::CStrRef label = FTL::CStrRef()
        );

      virtual Node * constructNodeFromPreset(
        Graph * graph,
        FTL::CStrRef name,
        FTL::CStrRef preset
        ) = 0;

      virtual Connection * constructConnection(
        Graph * graph,
        ConnectionTarget * src,
        ConnectionTarget * dst
        );
    };

  };

};

#endif // __UI_GraphView_GraphFactory__
