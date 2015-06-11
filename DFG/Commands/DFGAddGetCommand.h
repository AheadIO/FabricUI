// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGAddGetCommand__
#define __UI_DFG_DFGAddGetCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGAddGetCommand: public DFGCommand
    {
    public:

      DFGAddGetCommand(
        DFGController * controller,
        FTL::StrRef name,
        FTL::StrRef path,
        QPointF pos
        );

      virtual const char * getName() const { return "dfgAddGet"; }
      virtual const char * getShortDesc() const { return "Adds a get to the graph."; }
      virtual const char * getFullDesc() const { return "Adds a get to the graph using a given get name and a path."; }

      char const * getVarName() const;
      char const * getVarPath() const;
      char const * getNodePath() const;
      QPointF getPos() const;
      GraphView::Node * getNode();

    protected:
      
      virtual bool invoke();

    private:

      std::string m_varName;
      std::string m_varPath;
      std::string m_nodePath;
      QPointF m_pos;
    };

  };

};

#endif // __UI_DFG_DFGAddGetCommand__
