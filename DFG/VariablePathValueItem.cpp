// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "VariablePathValueItem.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

VariablePathValueItem::VariablePathValueItem(
  QString name,
  TreeView::TreeEditorFactory * factory,
  FabricCore::Client * client,
  FabricCore::DFGBinding binding,
  QWidget * parent,
  FabricCore::RTVal value,
  QString label,
  bool enabled
  )
: ValueEditor::ValueItem(name, factory, client, parent, value, label, enabled)
{
  m_binding = binding;
}
