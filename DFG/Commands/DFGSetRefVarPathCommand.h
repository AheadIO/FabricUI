// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGSetRefVarPathCommand__
#define __UI_DFG_DFGSetRefVarPathCommand__

#include "DFGCommand.h"
#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGSetRefVarPathCommand: public DFGCommand
    {
    public:

      DFGSetRefVarPathCommand(DFGController * controller, char const * path, char const * varPath);

      virtual const char * getName() const { return "dfgSetRefVarPath"; }
      virtual const char * getShortDesc() const { return "Sets the new variable path for a get or set node."; }
      virtual const char * getFullDesc() const { return "Sets the new variable path for a get or set node."; }

      const char * getPath() const;
      const char * getVarPath() const;

    protected:
      
      virtual bool invoke();

    private:

      std::string m_path;
      std::string m_varPath;

    };

  };

};

#endif // __UI_DFG_DFGSetRefVarPathCommand__
