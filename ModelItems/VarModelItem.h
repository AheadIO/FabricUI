//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ModelItems/NodeModelItem.h>

namespace FabricUI
{

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Access graph variables
    class VarModelItem : public NodeModelItem
    {
    public:

      VarModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName
        );
      ~VarModelItem();

      virtual BaseModelItem* createChild( FTL::CStrRef name );

      virtual int getInOut() /*override*/;

      virtual QVariant getValue() /*override*/;

    protected:

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;
    };
  }
}
