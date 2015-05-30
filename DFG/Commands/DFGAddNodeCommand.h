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

      DFGAddNodeCommand(DFGController * controller, QString path, QString preset, QPointF pos);

      virtual const char * getName() const { return "dfgAddNode"; }
      virtual const char * getShortDesc() const { return "Adds a node to the graph."; }
      virtual const char * getFullDesc() const { return "Adds a node to the graph using a given node type and a preset name."; }

      std::string getPath() const;
      std::string getInstPath() const;
      std::string getPreset() const;
      QPointF getPos() const;
      GraphView::Node * getNode();

    protected:
      
      virtual bool invoke();
      virtual bool undo();
      virtual bool redo();

    private:

      std::string m_path;
      std::string m_preset;
      std::string m_instPath;
      QPointF m_pos;
    };

  };

};

#endif // __UI_DFG_DFGAddNodeCommand__
