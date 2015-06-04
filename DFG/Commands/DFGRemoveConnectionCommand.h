// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGRemoveConnectionCommand__
#define __UI_DFG_DFGRemoveConnectionCommand__

#include "DFGCommand.h"
#include <GraphView/Port.h>

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGRemoveConnectionCommand: public DFGCommand
    {
    public:

      DFGRemoveConnectionCommand(DFGController * controller, const char * srcPath, const char * dstPath);

      virtual const char * getName() const { return "dfgRemoveConnection"; }
      virtual const char * getShortDesc() const { return "Removes a connection between two ports in the graph."; }
      virtual const char * getFullDesc() const { return "Removes a connection between two ports in the graph."; }

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

#endif // __UI_DFG_DFGRemoveConnectionCommand__
