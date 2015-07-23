// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "PresetTreeItem.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

PresetTreeItem::PresetTreeItem(
  FTL::CStrRef path,
  FTL::CStrRef name
  )
  : TreeView::TreeItem( name )
  , m_path( path )
{
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
