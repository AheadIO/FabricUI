// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_RemovePinCommand__
#define __UI_GraphView_RemovePinCommand__

#include "../Controller.h"
#include "ControllerCommand.h"

namespace FabricUI
{

  namespace GraphView
  {
    class RemovePinCommand : public ControllerCommand
    {
    public:

      RemovePinCommand(Controller * controller, Pin * pin);
      virtual ~RemovePinCommand();

      virtual const char * getName() const { return "RemovePin"; }
      virtual const char * getShortDesc() const { return "Removes a pin to from node in the graph."; }
      virtual const char * getFullDesc() const { return "Removes a pin to from node in the graph."; }

      virtual Node * getNode();
      virtual Pin * getPin();

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      QString m_path;
      QString m_name;
      PortType m_pType;
      QColor m_color;
      QString m_dataType;

    };

  };

};

#endif // __UI_GraphView_RemovePinCommand__
