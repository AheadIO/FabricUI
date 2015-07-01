// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGAddBackDropCommand__
#define __UI_DFG_DFGAddBackDropCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGAddBackDropCommand: public DFGCommand
    {
    public:

      DFGAddBackDropCommand(
        DFGController * controller,
        FTL::CStrRef title,
        QPointF pos
        );

      virtual const char * getName() const { return "dfgAddBackDrop"; }
      virtual const char * getShortDesc() const { return "Adds a backdrop to the graph."; }
      virtual const char * getFullDesc() const { return "Adds a backdrop to the graph with a given title."; }

      char const *getTitle() const
        { return m_title.c_str(); }
      char const *getNodePath() const
        { return m_nodePath.c_str(); }
      QPointF getPos() const
        { return m_pos; }
      GraphView::Node *getNode();

    protected:
      
      virtual bool invoke();

    private:

      std::string m_title;
      std::string m_nodePath;
      QPointF m_pos;
    };

  };

};

#endif // __UI_DFG_DFGAddBackDropCommand__
