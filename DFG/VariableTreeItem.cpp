// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "VariableTreeItem.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

VariableTreeItem::VariableTreeItem(char const * name, char const * path)
: TreeView::TreeItem(name)
{
  m_name = name;
  m_path = path;
}

Qt::ItemFlags VariableTreeItem::flags()
{
  Qt::ItemFlags flags = TreeView::TreeItem::flags();
  flags = flags | Qt::ItemIsDragEnabled;
  return flags;
}

QString VariableTreeItem::mimeDataAsText()
{
  return ("{\"type\": \"DFGVariable\", \"name\": \""+m_name+"\", \"path\": \""+m_path+"\"}").c_str();
}
