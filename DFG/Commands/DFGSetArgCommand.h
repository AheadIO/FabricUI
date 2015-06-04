// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGSetArgCommand__
#define __UI_DFG_DFGSetArgCommand__

#include "DFGCommand.h"
#include <GraphView/Port.h>

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGSetArgCommand: public DFGCommand
    {
    public:

      DFGSetArgCommand(DFGController * controller, char const * argName, char const * dataType, char const * json = "");
      DFGSetArgCommand(DFGController * controller, char const * argName, FabricCore::RTVal value);

      virtual const char * getName() const { return "dfgSetArg"; }
      virtual const char * getShortDesc() const { return "Binds a new value to a port."; }
      virtual const char * getFullDesc() const { return "Binds a new value to a port."; }

      const char * getArgName() const;
      const char * getDataType() const;
      const char * getJSON() const;
      FabricCore::RTVal getValue() const;

    protected:
      
      virtual bool invoke();

    private:

      std::string m_argName;
      std::string m_dataType;
      std::string m_json;
      FabricCore::RTVal m_value;

    };

  };

};

#endif // __UI_DFG_DFGSetArgCommand__
