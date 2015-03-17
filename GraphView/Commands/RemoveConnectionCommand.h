// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_RemoveConnectionCommand__
#define __UI_GraphView_RemoveConnectionCommand__

#include "../Controller.h"
#include "ControllerCommand.h"

namespace FabricUI
{

  namespace GraphView
  {
    class RemoveConnectionCommand : public ControllerCommand
    {
    public:

      RemoveConnectionCommand(Controller * controller, ConnectionTarget * src, ConnectionTarget * dst);
      virtual ~RemoveConnectionCommand();

      virtual const char * getName() const { return "RemoveConnection"; }
      virtual const char * getShortDesc() const { return "Removes a connection between two targets in the graph."; }
      virtual const char * getFullDesc() const { return "Removes a connection between two targets in the graph."; }

      virtual ConnectionTarget * getSrcTarget();
      virtual ConnectionTarget * getDstTarget();

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      bool m_srcIsPort;
      QString m_srcPath;
      QString m_srcName;
      bool m_dstIsPort;
      QString m_dstPath;
      QString m_dstName;

    };

  };

};

#endif // __UI_GraphView_RemoveConnectionCommand__
