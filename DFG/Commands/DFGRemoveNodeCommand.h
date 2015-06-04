// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGRemoveNodeCommand__
#define __UI_DFG_DFGRemoveNodeCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGRemoveNodeCommand: public DFGCommand
    {
    public:

      DFGRemoveNodeCommand(DFGController * controller, char const * path);

      virtual const char * getName() const { return "dfgRemoveNode"; }
      virtual const char * getShortDesc() const { return "Removes a node from the graph."; }
      virtual const char * getFullDesc() const { return "Removes a node from the graph."; }

      char const * getNodePath() const;

    protected:
      
      virtual bool invoke();

    private:

      std::string m_nodePath;

    };

  };

};

#endif // __UI_DFG_DFGRemoveNodeCommand__
