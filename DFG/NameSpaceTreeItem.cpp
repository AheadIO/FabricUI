// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NameSpaceTreeItem.h"
#include "PresetTreeItem.h"
#include <map>

using namespace FabricServices::DFGWrapper;
using namespace FabricUI;
using namespace FabricUI::DFG;

NameSpaceTreeItem::NameSpaceTreeItem(NameSpace nameSpace, QStringList filters)
: TreeView::TreeItem(nameSpace.getName().c_str(), "NameSpace")
, m_nameSpace(nameSpace)
, m_filters(filters)
{
  m_validated = false;
}

unsigned int NameSpaceTreeItem::numChildren()
{
  if(!m_validated)
  {
    {
      std::vector<NameSpace> nameSpaces = m_nameSpace.getNameSpaces();
      std::map<std::string, NameSpace> lookup;
      for(size_t i=0;i<nameSpaces.size();i++)
      {
        if(!includeChildName(nameSpaces[i].getName().c_str()))
          continue;
        lookup.insert(std::pair<std::string, NameSpace>(nameSpaces[i].getName(), nameSpaces[i]));
      }

      for(std::map<std::string, NameSpace>::iterator it=lookup.begin();it!=lookup.end();it++)
      {
        QStringList filters;
        QString search = QString(it->first.c_str()) + ".";
        for(size_t i=0;i<m_filters.length();i++)
        {
          if(!m_filters[i].startsWith(search))
            continue;
          filters.append(m_filters[i].right(m_filters[i].length() - search.length()));
        }
        addChild(new NameSpaceTreeItem(it->second, filters));
      }
    }

    {
      std::vector<Func> funcs = m_nameSpace.getFuncs();
      std::vector<Graph> graphs = m_nameSpace.getGraphs();
      std::map<std::string, Object> lookup;
      for(size_t i=0;i<funcs.size();i++)
      {
        if(!includeChildName(funcs[i].getName().c_str()))
          continue;
        lookup.insert(std::pair<std::string, Object>(funcs[i].getName(), funcs[i]));
      }
      for(size_t i=0;i<graphs.size();i++)
      {
        if(!includeChildName(graphs[i].getName().c_str()))
          continue;
        lookup.insert(std::pair<std::string, Object>(graphs[i].getName(), graphs[i]));
      }

      for(std::map<std::string, Object>::iterator it=lookup.begin();it!=lookup.end();it++)
        addChild(new PresetTreeItem(it->second));
    }


    m_validated = true;
  }
  return TreeView::TreeItem::numChildren();
}

QStringList NameSpaceTreeItem::filters() const
{
  return m_filters;
}

void NameSpaceTreeItem::setFilters(QStringList f)
{
  m_filters = f;
}

bool NameSpaceTreeItem::includeChildName(QString name)
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
