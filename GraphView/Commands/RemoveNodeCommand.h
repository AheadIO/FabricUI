// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_RemoveNodeCommand__
#define __UI_GraphView_RemoveNodeCommand__

#include "../Controller.h"
#include "ControllerCommand.h"

namespace FabricUI
{

  namespace GraphView
  {
    class RemoveNodeCommand : public ControllerCommand
    {
    public:

      RemoveNodeCommand(Controller * controller, char const * path);
      virtual ~RemoveNodeCommand();

      virtual const char * getName() const { return "RemoveNode"; }
      virtual const char * getShortDesc() const { return "Removes a node from the graph."; }
      virtual const char * getFullDesc() const { return "Removes a node from the graph."; }

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      virtual Node * getNode();

      std::string m_path;
      std::string m_preset;
      QPointF m_graphPos;

    };

  };

};

#endif // __UI_GraphView_RemoveNodeCommand__
