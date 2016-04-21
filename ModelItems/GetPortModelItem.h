//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_GETPORTMODELITEM_H
#define FABRICUI_MODELITEMS_GETPORTMODELITEM_H

#include <FabricUI/ModelItems/RefPortModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FTL/StrRef.h>

namespace FabricUI {

namespace DFG {
class DFGUICmdHandler;
} // namespace DFG

namespace ModelItems {

class NodePortItemMetadata;

//////////////////////////////////////////////////////////////////////////
// Basic ModelItem for accessing ports
class GetPortModelItem : public RefPortModelItem
{
public:

  GetPortModelItem(
    DFG::DFGUICmdHandler *dfgUICmdHandler,
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec,
    FTL::StrRef nodeName,
    FTL::StrRef portName
    );
  ~GetPortModelItem();

  /////////////////////////////////////////////////////////////////////////
  // Name
  /////////////////////////////////////////////////////////////////////////

  virtual FTL::CStrRef getName() /*override*/;

  virtual FabricUI::ValueEditor::ItemMetadata* getMetadata();
};

} // namespace ModelItems
} // namespace FabricUI

#endif // FABRICUI_MODELITEMS_GETPORTMODELITEM_H
