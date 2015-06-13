// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGRenameNodeCommand__
#define __UI_DFG_DFGRenameNodeCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGRenameNodeCommand: public DFGCommand
    {
    public:

      DFGRenameNodeCommand(DFGController * controller, char const * path, char const * title);

      virtual const char * getName() const { return "dfgRenameNode"; }
      virtual const char * getShortDesc() const { return "Renames a node from the graph."; }
      virtual const char * getFullDesc() const { return "Renames a node from the graph."; }

      char const * getPath() const;
      char const * getTitle() const;

    protected:
      
      virtual bool invoke();

    private:

      std::string m_nodePath;
      std::string m_newTitle;

    };

  };

};

#endif // __UI_DFG_DFGRenameNodeCommand__
