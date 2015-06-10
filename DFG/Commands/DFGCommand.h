// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGCommand__
#define __UI_DFG_DFGCommand__

#include <GraphView/Commands/ControllerCommand.h>

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGCommand: public GraphView::ControllerCommand
    {
    public:

      DFGCommand(DFGController * controller);

      virtual const char * getExecPath();

    protected:
      
      virtual bool undo();
      virtual bool redo();
    };

  };

};

#endif // __UI_DFG_DFGCommand__
