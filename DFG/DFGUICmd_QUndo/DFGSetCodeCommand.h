// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGSetCodeCommand__
#define __UI_DFG_DFGSetCodeCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGSetCodeCommand: public DFGCommand
    {
    public:

      DFGSetCodeCommand(DFGController * controller, char const * path, char const * code);

      virtual const char * getName() const { return "dfgSetCode"; }
      virtual const char * getShortDesc() const { return "Sets the code on a func node in the graph."; }
      virtual const char * getFullDesc() const { return "Sets the code on a func node in the graph."; }

      char const * getPath() const;
      char const * getCode() const;

    protected:
      
      virtual bool invoke();

    private:

      std::string m_execPath;
      std::string m_code;

    };

  };

};

#endif // __UI_DFG_DFGSetCodeCommand__
