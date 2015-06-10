// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGAddConnectionCommand__
#define __UI_DFG_DFGAddConnectionCommand__

#include "DFGCommand.h"
#include <FabricUI/GraphView/Port.h>

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGAddConnectionCommand: public DFGCommand
    {
    public:

      DFGAddConnectionCommand(DFGController * controller, const char * srcPath, const char * dstPath);

      virtual const char * getName() const { return "dfgAddConnection"; }
      virtual const char * getShortDesc() const { return "Adds a connection between two ports in the graph."; }
      virtual const char * getFullDesc() const { return "Adds a connection between two ports in the graph."; }

      const char * getSrcPath() const;
      const char * getDstPath() const;

    protected:
      
      virtual bool invoke();

    private:

      std::string m_srcPath;
      std::string m_dstPath;
    };

  };

};

#endif // __UI_DFG_DFGAddConnectionCommand__
