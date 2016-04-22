//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_VARMODELITEM_H
#define FABRICUI_MODELITEMS_VARMODELITEM_H

#include <FabricUI/ModelItems/NodeModelItem.h>
#include <FabricUI/ModelItems/VarItemMetadata.h>

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

      virtual FabricUI::ValueEditor::BaseModelItem* createChild( FTL::CStrRef name );

      virtual QVariant getValue() /*override*/;

      virtual FabricUI::ValueEditor::ItemMetadata *getMetadata() /*override*/;

    protected:

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;

    private:

      VarItemMetadata m_metadata;
    };
  }
}

#endif // FABRICUI_MODELITEMS_VARMODELITEM_H
