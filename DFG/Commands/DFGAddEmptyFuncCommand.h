// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGAddEmptyFuncCommand__
#define __UI_DFG_DFGAddEmptyFuncCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGAddEmptyFuncCommand: public DFGCommand
    {
    public:

      DFGAddEmptyFuncCommand(DFGController * controller, char const * path, char const * title, QPointF pos);

      virtual const char * getName() const { return "dfgAddEmptyFunc"; }
      virtual const char * getShortDesc() const { return "Adds an empty func node to the graph."; }
      virtual const char * getFullDesc() const { return "Adds an empty func node to the graph."; }

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

#endif // __UI_DFG_DFGAddEmptyFuncCommand__
