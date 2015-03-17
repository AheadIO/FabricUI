// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_RenamePortCommand__
#define __UI_GraphView_RenamePortCommand__

#include "../Controller.h"
#include "ControllerCommand.h"

namespace FabricUI
{

  namespace GraphView
  {
    class RenamePortCommand : public ControllerCommand
    {
    public:

      RenamePortCommand(Controller * controller, Port * port, QString title);
      virtual ~RenamePortCommand();

      virtual const char * getName() const { return "RenamePort"; }
      virtual const char * getShortDesc() const { return "Renames a port to from the graph."; }
      virtual const char * getFullDesc() const { return "Renames a port to from the graph."; }

      virtual Port * getPort();

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      QString m_path;
      PortType m_portType;
      QString m_newTitle;
      QString m_oldTitle;

    };

  };

};

#endif // __UI_GraphView_RenamePortCommand__
