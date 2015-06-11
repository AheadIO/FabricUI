// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "VariableListTreeItem.h"
#include "VariableTreeItem.h"
#include <map>

using namespace FabricUI;
using namespace FabricUI::DFG;

VariableListTreeItem::VariableListTreeItem(FabricCore::DFGHost const &coreDFGHost, FabricCore::DFGBinding const &coreDFGBinding, QStringList filters)
: TreeView::TreeItem("Variables", "VariableList")
, m_coreDFGHost(coreDFGHost)
, m_coreDFGBinding(coreDFGBinding)
, m_filters(filters)
{
}

unsigned int VariableListTreeItem::numChildren()
{
  if(TreeView::TreeItem::numChildren() == 0)
  {
    // add the default items
    addChild(new VariableTreeItem("New Var", "DFGVar"));
    addChild(new VariableTreeItem("Get Var", "DFGGet"));
    addChild(new VariableTreeItem("Set Var", "DFGSet"));
  }
  return TreeView::TreeItem::numChildren();
}

QStringList VariableListTreeItem::filters() const
{
  return m_filters;
}

void VariableListTreeItem::setFilters(QStringList f)
{
  m_filters = f;
}

bool VariableListTreeItem::includeChildName(QString name)
{
  QString start = name + ".";
  QString end = "." + name;

  for(unsigned int i=0;i<m_filters.length();i++)
  {
    if(m_filters[i] == name)
      return true;
    if(m_filters[i].startsWith(start))
      return true;
    if(m_filters[i].endsWith(end))
      return true;
  }
  return m_filters.length() == 0;
}
