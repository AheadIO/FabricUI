// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGCopyCommand__
#define __UI_DFG_DFGCopyCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGCopyCommand: public DFGCommand
    {
    public:

      DFGCopyCommand(DFGController * controller, QStringList nodePaths);

      virtual const char * getName() const { return "dfgCopy"; }
      virtual const char * getShortDesc() const { return "Copies nodes from  graph to the clipboard."; }
      virtual const char * getFullDesc() const { return "Copies nodes from  graph to the clipboard."; }

      std::vector<std::string> getNodePaths() const;
      std::string getJSON() const;

    protected:
      
      virtual bool invoke();
      virtual bool undo();
      virtual bool redo();

    private:

      std::vector<std::string> m_nodePaths;
      std::vector<char const *> m_nodePathsPtr;
      std::string m_json;
    };

  };

};

#endif // __UI_DFG_DFGCopyCommand__
