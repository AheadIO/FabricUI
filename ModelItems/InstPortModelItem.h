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

    //////////////////////////////////////////////////////////////////////////
    // Basic ModelItem for accessing ports
    class InstPortModelItem : public NodePortModelItem
    {
    public:

      InstPortModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName,
        FTL::StrRef portName
        );
      ~InstPortModelItem();

      virtual ItemMetadata *getMetadata() /*override*/;

      virtual bool canRenameItem();
      virtual void rename( const char* newName );

      virtual QVariant getValue();

      virtual bool hasDefault() /*override*/;
      virtual void resetToDefault() /*override*/;
    };
  }
}
