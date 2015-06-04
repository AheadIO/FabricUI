// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGExplodeNodeCommand__
#define __UI_DFG_DFGExplodeNodeCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGExplodeNodeCommand: public DFGCommand
    {
    public:

      DFGExplodeNodeCommand(DFGController * controller, char const * nodePath);

      virtual const char * getName() const { return "dfgExplodeNode"; }
      virtual const char * getShortDesc() const { return "Groups nodes in graph to a new subgraph."; }
      virtual const char * getFullDesc() const { return "Groups nodes in graph to a new subgraph."; }

      char const * getNodePath() const;
      std::vector<std::string> getNodeNames() const;

    protected:
      
      virtual bool invoke();

    private:

      std::string m_nodePath;
      std::vector<std::string> m_nodeNames;
    };

  };

};

#endif // __UI_DFG_DFGExplodeNodeCommand__
