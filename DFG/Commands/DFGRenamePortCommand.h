// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGRenamePortCommand__
#define __UI_DFG_DFGRenamePortCommand__

#include "DFGCommand.h"
#include <GraphView/Port.h>

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGRenamePortCommand: public DFGCommand
    {
    public:

      DFGRenamePortCommand(DFGController * controller, QString path, QString name);

      virtual const char * getName() const { return "dfgRenamePort"; }
      virtual const char * getShortDesc() const { return "Renames a port to from the graph."; }
      virtual const char * getFullDesc() const { return "Renames a port to from the graph."; }

      const char * getPath() const;
      const char * getPortName() const;
      const char * getResult() const;

    protected:
      
      virtual bool invoke();
      virtual bool undo();
      virtual bool redo();

    private:

      std::string m_path;
      std::string m_newName;
      std::string m_result;

    };

  };

};

#endif // __UI_DFG_DFGRenamePortCommand__
