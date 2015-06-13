// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_RenameNodeCommand__
#define __UI_GraphView_RenameNodeCommand__

#include <FTL/ArrayRef.h>

#include "../Controller.h"
#include "ControllerCommand.h"

namespace FabricUI
{

  namespace GraphView
  {
    class RenameNodeCommand : public ControllerCommand
    {
    public:

      RenameNodeCommand(Controller * controller, Node * node, char const * title);
      virtual ~RenameNodeCommand();

      virtual const char * getName() const { return "RenameNode"; }
      virtual const char * getShortDesc() const { return "Renames a node from the graph."; }
      virtual const char * getFullDesc() const { return "Renames a node from the graph."; }

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      virtual Node * getNode();

      std::string m_path;
      std::string m_newTitle;
      std::string m_oldTitle;

    };

  };

};

#endif // __UI_GraphView_RenameNodeCommand__
