// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NameSpaceTreeItem.h"
#include "PresetTreeItem.h"
#include <map>

using namespace FabricUI;
using namespace FabricUI::DFG;

NameSpaceTreeItem::NameSpaceTreeItem(FabricCore::DFGHost const &coreDFGHost, char const * name, char const * ns, QStringList filters)
: TreeView::TreeItem(name, "NameSpace")
, m_coreDFGHost(coreDFGHost)
, m_nameSpace(ns)
, m_filters(filters)
{
  m_validated = false;
}

unsigned int NameSpaceTreeItem::numChildren()
{
  if(!m_validated)
  {
    std::string prefix = m_nameSpace + ".";
    std::map<std::string, std::string> nameSpaceLookup;
    std::map<std::string, std::string> presetLookup;

    FabricCore::DFGStringResult jsonStr = m_coreDFGHost.getPresetDesc(m_nameSpace.c_str());
    FabricCore::Variant jsonVar = FabricCore::Variant::CreateFromJSON(jsonStr.getCString());
    const FabricCore::Variant * membersVar = jsonVar.getDictValue("members");

    for(FabricCore::Variant::DictIter memberIter(*membersVar); !memberIter.isDone(); memberIter.next())
    {
      std::string name = memberIter.getKey()->getStringData();
      const FabricCore::Variant * memberVar = memberIter.getValue();
      const FabricCore::Variant * objectTypeVar = memberVar->getDictValue("objectType");
      std::string objectType = objectTypeVar->getStringData();
      if(objectType == "Preset")
      {
        presetLookup.insert(std::pair<std::string, std::string>(name, prefix + name));
      }
      else if(objectType == "NameSpace")
      {
        nameSpaceLookup.insert(std::pair<std::string, std::string>(name, prefix + name));
      }
    }

    for(std::map<std::string, std::string>::iterator it=nameSpaceLookup.begin();it!=nameSpaceLookup.end();it++)
    {
      QStringList filters;
      QString search = QString(it->first.c_str()) + ".";
      for(size_t i=0;i<m_filters.length();i++)
      {
        if(!m_filters[i].startsWith(search))
          continue;
        filters.append(m_filters[i].right(m_filters[i].length() - search.length()));
      }
      if(filters.length() == 0 && m_filters.length() > 0)
        continue;
      addChild(new NameSpaceTreeItem(m_coreDFGHost, it->first.c_str(), it->second.c_str(), filters));
    }

    for(std::map<std::string, std::string>::iterator it=presetLookup.begin();it!=presetLookup.end();it++)
    {
      if(!includeChildName(it->first.c_str()))
        continue;
      addChild(new PresetTreeItem(it->second.c_str(), it->first.c_str()));
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
