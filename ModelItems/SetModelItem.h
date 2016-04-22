//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_SETMODELITEM_H
#define FABRICUI_MODELITEMS_SETMODELITEM_H

#include <FabricUI/ModelItems/RefModelItem.h>
#include <FabricUI/ModelItems/RefItemMetadata.h>

namespace FabricUI {
namespace ModelItems {

class SetModelItem : public RefModelItem
{
public:

  SetModelItem(
    DFG::DFGUICmdHandler *dfgUICmdHandler,
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec,
    FTL::StrRef nodeName
    );
  ~SetModelItem();

  virtual FabricUI::ValueEditor::BaseModelItem* createChild( FTL::CStrRef name ) /*override*/;
};

} // namespace ModelItems
} // namespace FabricUI

#endif // FABRICUI_MODELITEMS_SETMODELITEM_H
