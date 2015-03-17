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

      DFGAddEmptyGraphCommand(DFGController * controller, QString path, QString title, QPointF pos);

      virtual const char * getName() const { return "dfgAddEmptyGraph"; }
      virtual const char * getShortDesc() const { return "Adds an empty graph node to the graph."; }
      virtual const char * getFullDesc() const { return "Adds an empty graph node to the graph."; }

      std::string getPath() const;
      std::string getTitle() const;
      QPointF getPos() const;
      std::string getNodePath() const;
      GraphView::Node * getNode();

    protected:
      
      virtual bool invoke();
      virtual bool undo();
      virtual bool redo();

    private:

      std::string m_path;
      std::string m_title;
      std::string m_nodePath;
      QPointF m_pos;
    };

  };

};

#endif // __UI_DFG_DFGAddEmptyGraphCommand__
