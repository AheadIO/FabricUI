//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_REFMODELITEM_H
#define FABRICUI_MODELITEMS_REFMODELITEM_H

#include <FabricUI/ModelItems/NodeModelItem.h>
#include <FabricUI/ModelItems/RefItemMetadata.h>

namespace FabricUI {
namespace ModelItems {

class RefModelItem : public NodeModelItem
{
public:

  RefModelItem(
    DFG::DFGUICmdHandler *dfgUICmdHandler,
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec,
    FTL::StrRef nodeName
    );
  ~RefModelItem();

  virtual bool isRef() const /*override*/ { return true; }

  virtual int getNumChildren() /*override*/;
  virtual FTL::CStrRef getChildName( int i ) /*override*/;

  virtual QVariant getValue() /*override*/;

  virtual FabricUI::ValueEditor::ItemMetadata *getMetadata() /*override*/;

protected:

  virtual void setValue(
    QVariant var,
    bool commit,
    QVariant valueAtInteractionBegin
    ) /*override*/;

private:

  RefItemMetadata m_metadata;
};

} // namespace ModelItems
} // namespace FabricUI

#endif // FABRICUI_MODELITEMS_REFMODELITEM_H
