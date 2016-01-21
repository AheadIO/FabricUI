//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FabricUI/ModelItems/NodeModelItem.h>
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
    class InstModelItem : public NodeModelItem
    {
    public:
      
      InstModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName
        );
      ~InstModelItem();

      virtual BaseModelItem* createChild( FTL::CStrRef name );

      virtual QVariant GetValue();

    protected:

      virtual void SetValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;
    };
  }
}
