// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGAddConnectionCommand__
#define __UI_DFG_DFGAddConnectionCommand__

#include "DFGCommand.h"
#include <GraphView/Port.h>

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGAddConnectionCommand: public DFGCommand
    {
    public:

      DFGAddConnectionCommand(DFGController * controller, const char * srcPath, const char * dstPath, bool srcAsPin, bool dstAsPin);

      virtual const char * getName() const { return "dfgAddConnection"; }
      virtual const char * getShortDesc() const { return "Adds a connection between two ports in the graph."; }
      virtual const char * getFullDesc() const { return "Adds a connection between two ports in the graph."; }

      const char * getSrcPath() const;
      const char * getDstPath() const;
      bool srcIsPin() const;
      bool dstIsPin() const;

    protected:
      
      virtual bool invoke();
      virtual bool undo();
      virtual bool redo();

    private:

      bool m_srcIsPin;
      bool m_dstIsPin;
      std::string m_srcPath;
      std::string m_dstPath;
      std::string m_srcNodePath;
      std::string m_dstNodePath;
      std::string m_srcPinTitle;
      std::string m_dstPinTitle;
    };

  };

};

#endif // __UI_DFG_DFGAddConnectionCommand__
