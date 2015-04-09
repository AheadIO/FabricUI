// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGPasteCommand__
#define __UI_DFG_DFGPasteCommand__

#include "DFGCommand.h"

#include "../DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    class DFGPasteCommand: public DFGCommand
    {
    public:

      DFGPasteCommand(DFGController * controller, QString json = "");

      virtual const char * getName() const { return "dfgPaste"; }
      virtual const char * getShortDesc() const { return "Pastes nodes from clipboard to the graph."; }
      virtual const char * getFullDesc() const { return "Pastes nodes from clipboard to the graph."; }

      std::string getJSON() const;
      std::vector<std::string> getNodePaths() const;

    protected:
      
      virtual bool invoke();
      virtual bool undo();
      virtual bool redo();

    private:

      std::string m_json;
      std::vector<std::string> m_nodePaths;
    };

  };

};

#endif // __UI_DFG_DFGPasteCommand__
