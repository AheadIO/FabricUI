// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGAddEmptyGraphCommand__
#define __UI_DFG_DFGAddEmptyGraphCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGAddEmptyGraphCommand: public DFGCommand
    {
    public:

      DFGAddEmptyGraphCommand(DFGController * controller, char const * path, char const * title, QPointF pos);

      virtual const char * getName() const { return "dfgAddEmptyGraph"; }
      virtual const char * getShortDesc() const { return "Adds an empty graph node to the graph."; }
      virtual const char * getFullDesc() const { return "Adds an empty graph node to the graph."; }

      char const * getPath() const;
      char const * getTitle() const;
      QPointF getPos() const;
      char const * getInstPath() const;
      GraphView::Node * getNode();

    protected:
      
      virtual bool invoke();

    private:

      std::string m_path;
      std::string m_title;
      std::string m_instPath;
      QPointF m_pos;
    };

  };

};

#endif // __UI_DFG_DFGAddEmptyGraphCommand__
