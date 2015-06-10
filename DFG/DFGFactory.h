// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGFactory__
#define __UI_DFG_DFGFactory__

#include <FabricUI/GraphView/GraphFactory.h>
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
        FTL::CStrRef path,
        FTL::CStrRef preset
        );

    private:

      DFGConfig m_config;
    };

  };

};

#endif // __UI_DFG_DFGFactory__
