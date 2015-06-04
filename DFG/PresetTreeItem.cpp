// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "PresetTreeItem.h"

PresetTreeItem::PresetTreeItem(char const * path, char const * name)
: TreeView::TreeItem(name, "Preset")
{
  m_path = path;
}

Qt::ItemFlags PresetTreeItem::flags()
{
  Qt::ItemFlags flags = TreeView::TreeItem::flags();
  flags = flags | Qt::ItemIsDragEnabled;
  return flags;
}

QString PresetTreeItem::mimeDataAsText()
{
  return ("{\"type\": \"DFGPreset\", \"path\": \""+m_path+"\"}").c_str();
}
