// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_AddConnectionCommand__
#define __UI_GraphView_AddConnectionCommand__

#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/Commands/ControllerCommand.h>

namespace FabricUI
{

  namespace GraphView
  {
    class AddConnectionCommand : public ControllerCommand
    {
    public:

      AddConnectionCommand(Controller * controller, ConnectionTarget * src, ConnectionTarget * dst);
      virtual ~AddConnectionCommand();

      virtual const char * getName() const { return "AddConnection"; }
      virtual const char * getShortDesc() const { return "Adds a connection between two targets in the graph."; }
      virtual const char * getFullDesc() const { return "Adds a connection between two targets in the graph."; }

      virtual ConnectionTarget * getSrcTarget();
      virtual ConnectionTarget * getDstTarget();

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      bool m_srcIsPort;
      std::string m_srcPath;
      std::string m_srcName;
      bool m_dstIsPort;
      std::string m_dstPath;
      std::string m_dstName;

    };

  };

};

#endif // __UI_GraphView_AddConnectionCommand__
