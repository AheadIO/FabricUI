// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "PresetTreeItem.h"

using namespace FabricServices::DFGWrapper;
using namespace FabricUI;
using namespace FabricUI::DFG;

PresetTreeItem::PresetTreeItem(Object preset)
: TreeView::TreeItem(preset.getName().c_str(), "Preset")
{
  m_path = preset.getPath().c_str();
}

Qt::ItemFlags PresetTreeItem::flags()
{
  Qt::ItemFlags flags = TreeView::TreeItem::flags();
  flags = flags | Qt::ItemIsDragEnabled;
  return flags;
}

QString PresetTreeItem::mimeDataAsText()
{
  return "{\"type\": \"DFGPreset\", \"path\": \""+m_path+"\"}";
}
