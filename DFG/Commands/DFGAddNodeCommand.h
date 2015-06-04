// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGAddNodeCommand__
#define __UI_DFG_DFGAddNodeCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGAddNodeCommand: public DFGCommand
    {
    public:

      DFGAddNodeCommand(DFGController * controller, char const * path, char const * preset, QPointF pos);

      virtual const char * getName() const { return "dfgAddNode"; }
      virtual const char * getShortDesc() const { return "Adds a node to the graph."; }
      virtual const char * getFullDesc() const { return "Adds a node to the graph using a given node type and a preset name."; }

      char const * getPath() const;
      char const * getInstPath() const;
      char const * getPreset() const;
      QPointF getPos() const;
      GraphView::Node * getNode();

    protected:
      
      virtual bool invoke();

    private:

      std::string m_path;
      std::string m_preset;
      std::string m_instPath;
      QPointF m_pos;
    };

  };

};

#endif // __UI_DFG_DFGAddNodeCommand__
