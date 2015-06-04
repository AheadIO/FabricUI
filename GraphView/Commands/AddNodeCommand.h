// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_AddNodeCommand__
#define __UI_GraphView_AddNodeCommand__

#include "../Controller.h"
#include "ControllerCommand.h"

namespace FabricUI
{

  namespace GraphView
  {
    class AddNodeCommand : public ControllerCommand
    {
    public:

      AddNodeCommand(Controller * controller, char const * path, char const * preset, QPointF pos);
      virtual ~AddNodeCommand();

      virtual const char * getName() const { return "AddNode"; }
      virtual const char * getShortDesc() const { return "Adds a node to the graph."; }
      virtual const char * getFullDesc() const { return "Adds a node to the graph using a given node type and a preset name."; }

      virtual Node * getNode();

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      std::string m_path;
      std::string m_preset;
      QPointF m_graphPos;
      bool m_useTopLeftPos;

    };

  };

};

#endif // __UI_GraphView_AddNodeCommand__
