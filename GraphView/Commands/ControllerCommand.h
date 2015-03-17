// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_ControllerCommand__
#define __UI_GraphView_ControllerCommand__

#include "../Controller.h"
#include "../Graph.h"
#include "../Node.h"
#include "../Pin.h"
#include "../Port.h"
#include "../Connection.h"
#include "../ConnectionTarget.h"

namespace FabricUI
{

  namespace GraphView
  {
    class ControllerCommand : public FabricServices::Commands::Command
    {
    public:

      ControllerCommand(Controller * controller);
      virtual ~ControllerCommand();

      Controller * controller();

    private:

      Controller * m_controller;
    };

  };

};

#endif // __UI_GraphView_ControllerCommand__
