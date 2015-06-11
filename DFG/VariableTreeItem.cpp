// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "VariableTreeItem.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

VariableTreeItem::VariableTreeItem(char const * name, char const * type)
: TreeView::TreeItem(name, "Variable")
{
  m_name = name;
  m_type = type;
}

Qt::ItemFlags VariableTreeItem::flags()
{
  Qt::ItemFlags flags = TreeView::TreeItem::flags();
  flags = flags | Qt::ItemIsDragEnabled;
  return flags;
}

QString VariableTreeItem::mimeDataAsText()
{
  return ("{\"type\": \""+m_type+"\", \"name\": \""+m_name+"\"}").c_str();
}
