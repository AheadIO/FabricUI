// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGAddPortCommand__
#define __UI_DFG_DFGAddPortCommand__

#include "DFGCommand.h"
#include <FabricUI/GraphView/Port.h>

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGAddPortCommand: public DFGCommand
    {
    public:

      DFGAddPortCommand(
        DFGController * controller,
        FTL::StrRef path,
        FTL::StrRef name,
        GraphView::PortType pType,
        FTL::StrRef dataType
        );

      virtual const char * getName() const { return "dfgAddPort"; }
      virtual const char * getShortDesc() const { return "Adds a port."; }
      virtual const char * getFullDesc() const { return "Adds a port on a graph as an external port."; }

      const char * getPath() const;
      const char * getPortName() const;
      GraphView::PortType getPortType() const;
      const char * getDataType() const;
      const char * getPortPath() const;

    protected:
      
      virtual bool invoke();

    private:

      std::string m_path;
      std::string m_portTitle;
      std::string m_portPath;
      GraphView::PortType m_portType;
      std::string m_dataType;

    };

  };

};

#endif // __UI_DFG_DFGAddPortCommand__
