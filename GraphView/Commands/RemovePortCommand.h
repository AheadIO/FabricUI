// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_RemovePortCommand__
#define __UI_GraphView_RemovePortCommand__

#include "../Controller.h"
#include "ControllerCommand.h"

namespace FabricUI
{

  namespace GraphView
  {
    class RemovePortCommand : public ControllerCommand
    {
    public:

      RemovePortCommand(Controller * controller, Port * port);
      virtual ~RemovePortCommand();

      virtual const char * getName() const { return "RemovePort"; }
      virtual const char * getShortDesc() const { return "Removes a port to from the graph."; }
      virtual const char * getFullDesc() const { return "Removes a port to from the graph."; }

      virtual Port * getPort();

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      QString m_name;
      PortType m_pType;
      QColor m_color;
      QString m_dataType;

    };

  };

};

#endif // __UI_GraphView_RemovePortCommand__
