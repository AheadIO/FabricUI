//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FabricUI/ModelItems/RootModelItem.h>
#include <FTL/StrRef.h>

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{
namespace DFG {
class DFGUICmdHandler;
}

  namespace ModelItems
  {
    class NodeModelItem : public RootModelItem
    {
    protected:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      std::string m_nodeName;
  
    public:
      
      NodeModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName
        );
      ~NodeModelItem();

      virtual bool isNode() const /*override*/ { return true; }

      FTL::CStrRef getNodeName()
        { return m_nodeName; }

      virtual int getNumChildren() /*override*/;
      virtual FTL::CStrRef getChildName( int i );

      /////////////////////////////////////////////////////////////////////////
      // Name
      /////////////////////////////////////////////////////////////////////////

      virtual FTL::CStrRef getName() /*override*/;

      virtual bool canRename() /*override*/;

      virtual void rename( FTL::CStrRef newName ) /*override*/;

      virtual void onRenamed(
        FTL::CStrRef oldNodeName,
        FTL::CStrRef newNodeName
        ) /*override*/;

      // Metadata

      virtual ItemMetadata* getMetadata();
      virtual void setMetadataImp( const char* key, 
                                const char* value, 
                                bool canUndo )/*override*/;
    };
  }
}
