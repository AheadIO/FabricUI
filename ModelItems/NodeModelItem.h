//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
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

      virtual bool matchesPath(
        FTL::StrRef execPath,
        FTL::StrRef nodeName
        ) /*override*/;

      virtual int getNumChildren() /*override*/;
      virtual FTL::CStrRef getChildName( int i );

      virtual FTL::CStrRef getName();
      virtual void renameItem( const char* newName );

      virtual BaseModelItem *onNodePortRenamed(
        FTL::CStrRef execPath,
        FTL::CStrRef nodeName,
        FTL::CStrRef oldNodePortName,
        FTL::CStrRef newNodePortName
        ) /*override*/;

      virtual BaseModelItem *onNodeRenamed(
        FTL::CStrRef execPath,
        FTL::CStrRef oldNodeName,
        FTL::CStrRef newNodeName
        ) /*override*/;

      virtual ItemMetadata* getMetadata();
      virtual void setMetadataImp( const char* key, 
                                const char* value, 
                                bool canUndo )/*override*/;
    };
  }
}
