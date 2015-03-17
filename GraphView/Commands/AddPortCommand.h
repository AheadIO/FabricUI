// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_AddPortCommand__
#define __UI_GraphView_AddPortCommand__

#include "../Controller.h"
#include "ControllerCommand.h"

namespace FabricUI
{

  namespace GraphView
  {
    class AddPortCommand : public ControllerCommand
    {
    public:

      AddPortCommand(Controller * controller, QString name, PortType pType, QColor color, QString dataType);
      virtual ~AddPortCommand();

      virtual const char * getName() const { return "AddPort"; }
      virtual const char * getShortDesc() const { return "Adds a port to a node in the graph."; }
      virtual const char * getFullDesc() const { return "Adds a port to a node in the graph using a given a port name, type, color, label and datatype."; }

      virtual Port * getPort();

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      QString m_name;
      PortType m_pType;
      QColor m_color;
      QString m_label;
      QString m_dataType;

    };

  };

};

#endif // __UI_GraphView_AddPortCommand__
