// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGSetDefaultValueCommand__
#define __UI_DFG_DFGSetDefaultValueCommand__

#include "DFGCommand.h"
#include <FabricUI/GraphView/Port.h>

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGSetDefaultValueCommand: public DFGCommand
    {
    public:

      DFGSetDefaultValueCommand(DFGController * controller, char const * pathFromRoot, FabricCore::RTVal value);

      virtual const char * getName() const { return "dfgSetDefaultValue"; }
      virtual const char * getShortDesc() const { return "Sets a new default value on a port / pin."; }
      virtual const char * getFullDesc() const { return "Sets a new default value on a port / pin."; }

      const char * getPath() const;
      const char * getDataType() const;
      const char * getJSON() const;
      FabricCore::RTVal getValue() const;

    protected:
      
      virtual bool invoke();

    private:

      std::string m_path;
      std::string m_dataType;
      std::string m_json;
      FabricCore::RTVal m_value;

    };

  };

};

#endif // __UI_DFG_DFGSetDefaultValueCommand__
