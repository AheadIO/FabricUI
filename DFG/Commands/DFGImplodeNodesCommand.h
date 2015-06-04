// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGImplodeNodesCommand__
#define __UI_DFG_DFGImplodeNodesCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGImplodeNodesCommand: public DFGCommand
    {
    public:

      DFGImplodeNodesCommand(DFGController * controller, QString desiredName, QStringList nodePaths);

      virtual const char * getName() const { return "dfgImplodeNodes"; }
      virtual const char * getShortDesc() const { return "Groups nodes in graph to a new subgraph."; }
      virtual const char * getFullDesc() const { return "Groups nodes in graph to a new subgraph."; }

      std::vector<std::string> getNodePaths() const;
      std::string getDesiredName() const;
      std::string getInstName() const;

    protected:
      
      virtual bool invoke();

    private:

      std::vector<std::string> m_nodePaths;
      std::vector<char const *> m_nodePathsPtr;
      std::string m_desiredName;
      std::string m_instName;
    };

  };

};

#endif // __UI_DFG_DFGImplodeNodesCommand__
