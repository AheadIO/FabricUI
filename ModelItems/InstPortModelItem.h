//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_INSTPORTMODELITEM_H
#define FABRICUI_MODELITEMS_INSTPORTMODELITEM_H

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

      /////////////////////////////////////////////////////////////////////////
      // Name
      /////////////////////////////////////////////////////////////////////////

      virtual FTL::CStrRef getName() /*override*/;

      virtual bool canRename() /*override*/;

      virtual void rename( FTL::CStrRef newName ) /*override*/;

      virtual void onRenamed(
        FTL::CStrRef oldName,
        FTL::CStrRef newName
        ) /*override*/;

      /////////////////////////////////////////////////////////////////////////
      // Others
      /////////////////////////////////////////////////////////////////////////

      virtual FabricUI::ValueEditor::ItemMetadata *getMetadata() /*override*/;

      virtual QVariant getValue();

      virtual bool hasDefault() /*override*/;
      virtual void resetToDefault() /*override*/;
    };
  }
}

#endif // FABRICUI_MODELITEMS_INSTPORTMODELITEM_H
