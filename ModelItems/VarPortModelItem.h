//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ModelItems/NodePortModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FTL/StrRef.h>

namespace FabricUI
{

namespace DFG {
class DFGUICmdHandler;
}

  namespace ModelItems
  {

    class NodePortItemMetadata;

    //////////////////////////////////////////////////////////////////////////
    // Basic ModelItem for accessing ports
    class VarPortModelItem : public NodePortModelItem
    {
    public:

      VarPortModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName,
        FTL::StrRef portName
        );
      ~VarPortModelItem();

      virtual FTL::CStrRef getName() /*override*/;
      virtual bool canRenameItem() /*override*/;
      virtual void RenameItem( const char* newName ) /*override*/;
      
      virtual QVariant GetValue();

      virtual bool hasDefault() /*override*/;
      virtual void resetToDefault() /*override*/;
    };
  }
}
