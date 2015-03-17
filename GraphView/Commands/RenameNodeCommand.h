// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_RenameNodeCommand__
#define __UI_GraphView_RenameNodeCommand__

#include "../Controller.h"
#include "ControllerCommand.h"

namespace FabricUI
{

  namespace GraphView
  {
    class RenameNodeCommand : public ControllerCommand
    {
    public:

      RenameNodeCommand(Controller * controller, Node * node, QString title);
      virtual ~RenameNodeCommand();

      virtual const char * getName() const { return "RenameNode"; }
      virtual const char * getShortDesc() const { return "Renames a node from the graph."; }
      virtual const char * getFullDesc() const { return "Renames a node from the graph."; }

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      virtual Node * getNode();

      QString m_path;
      QString m_newTitle;
      QString m_oldTitle;

    };

  };

};

#endif // __UI_GraphView_RenameNodeCommand__
