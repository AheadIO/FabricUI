// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "VariablePathValueItem.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

VariablePathValueItem::VariablePathValueItem(
  FTL::StrRef name,
  TreeView::TreeEditorFactory * factory,
  FabricCore::Client * client,
  FabricCore::DFGBinding binding,
  FTL::CStrRef execPath,
  QWidget * parent,
  FabricCore::RTVal value,
  FTL::StrRef label,
  bool enabled
  )
  : ValueEditor::ValueItem(
    name,
    factory,
    client,
    parent,
    value,
    label,
    enabled
    )
{
  m_binding = binding;
  m_execPath = execPath.c_str();
}
