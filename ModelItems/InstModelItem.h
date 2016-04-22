//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_INSTMODELITEM_H
#define FABRICUI_MODELITEMS_INSTMODELITEM_H

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

      virtual FabricUI::ValueEditor::BaseModelItem* createChild( FTL::CStrRef name );

      virtual QVariant getValue();

    protected:

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;
    };
  }
}

#endif // FABRICUI_MODELITEMS_INSTMODELITEM_H
