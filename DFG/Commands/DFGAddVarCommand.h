// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGAddVarCommand__
#define __UI_DFG_DFGAddVarCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGAddVarCommand: public DFGCommand
    {
    public:

      DFGAddVarCommand(
        DFGController * controller,
        FTL::StrRef name,
        QPointF pos
        );

      virtual const char * getName() const { return "dfgAddVar"; }
      virtual const char * getShortDesc() const { return "Adds a variable to the graph."; }
      virtual const char * getFullDesc() const { return "Adds a variable to the graph using a given variable name and a path."; }

      char const * getVarName() const;
      char const * getNodePath() const;
      QPointF getPos() const;
      GraphView::Node * getNode();

    protected:
      
      virtual bool invoke();

    private:

      std::string m_varName;
      std::string m_nodePath;
      QPointF m_pos;
    };

  };

};

#endif // __UI_DFG_DFGAddVarCommand__
