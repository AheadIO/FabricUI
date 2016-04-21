//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_REFPORTMODELITEM_H
#define FABRICUI_MODELITEMS_REFPORTMODELITEM_H

#include <FabricUI/ModelItems/NodePortModelItem.h>
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
class RefPortModelItem : public NodePortModelItem
{
public:

  RefPortModelItem(
    DFG::DFGUICmdHandler *dfgUICmdHandler,
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec,
    FTL::StrRef nodeName,
    FTL::StrRef portName
    );
  ~RefPortModelItem();

  /////////////////////////////////////////////////////////////////////////
  // Name
  /////////////////////////////////////////////////////////////////////////

  virtual bool canRename() /*override*/;

  virtual void rename( FTL::CStrRef newName ) /*override*/;
  
  virtual void onRenamed(
    FTL::CStrRef oldName,
    FTL::CStrRef newName
    ) /*override*/;

  /////////////////////////////////////////////////////////////////////////
  // Others
  /////////////////////////////////////////////////////////////////////////

  virtual QVariant getValue();

  virtual bool hasDefault() /*override*/;
  virtual void resetToDefault() /*override*/;
};

} // namespace ModelItems
} // namespace FabricUI

#endif // FABRICUI_MODELITEMS_REFPORTMODELITEM_H
