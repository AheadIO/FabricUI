// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGRemovePortCommand__
#define __UI_DFG_DFGRemovePortCommand__

#include "DFGCommand.h"
#include <GraphView/Port.h>

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGRemovePortCommand: public DFGCommand
    {
    public:

      DFGRemovePortCommand(DFGController * controller, QString execPath, QString name);

      virtual const char * getName() const { return "dfgRemovePort"; }
      virtual const char * getShortDesc() const { return "Removes a port to from the graph."; }
      virtual const char * getFullDesc() const { return "Removes a port to from the graph."; }

      const char * getExecPath() const;
      const char * getPortName() const;

    protected:
      
      virtual bool invoke();
      virtual bool undo();
      virtual bool redo();

    private:

      std::string m_execPath;
      std::string m_portTitle;

    };

  };

};

#endif // __UI_DFG_DFGRemovePortCommand__
