// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGFactory__
#define __UI_DFG_DFGFactory__

#include <GraphView/GraphFactory.h>
#include "DFGConfig.h"

namespace FabricUI
{

  namespace DFG
  {

    class DFGFactory : public GraphView::GraphFactory
    {

    public:

      DFGFactory(const DFGConfig & config = DFGConfig());

      virtual GraphView::Node * constructNodeFromPreset(
        GraphView::Graph * graph,
        char const *path,
        char const *preset
        );

    private:

      DFGConfig m_config;
    };

  };

};

#endif // __UI_DFG_DFGFactory__
