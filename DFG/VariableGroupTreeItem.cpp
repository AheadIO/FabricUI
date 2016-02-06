// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "VariableGroupTreeItem.h"
#include "VariableTreeItem.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

VariableGroupTreeItem::VariableGroupTreeItem(char const * name, char const * path, FTL::JSONObject const * jsonObject, QStringList filters)
: TreeView::TreeItem(name)
, m_filters(filters)
{
  m_name = name;
  m_path = path;
  m_jsonObject = jsonObject;
}

unsigned int VariableGroupTreeItem::numChildren()
{
  if(TreeView::TreeItem::numChildren() == 0)
  {
    if(m_jsonObject)
    {
      FTL::JSONObject const * subsObject = m_jsonObject->maybeGetObject( FTL_STR("subs") );
      if(subsObject)
      {
        QStringList names;
        for(FTL::JSONObject::const_iterator it = subsObject->begin();
          it != subsObject->end(); it++
          )
        {
          FTL::JSONObject const * subGraph = it->second->maybeCast<FTL::JSONObject>();
          if(!subGraph)
            continue;
          names.append(it->first.c_str());
        }      

        names.sort();
        for(int i=0;i<names.length();i++)
        {
          std::string name = names[i].toUtf8().constData();
          std::string path = m_path;
          if(path.length() != 0)
            path += ".";
          path += name;
          FTL::JSONObject const * subGraph = subsObject->maybeGetObject(name.c_str());
          addChild(new VariableGroupTreeItem(name.c_str(), path.c_str(), subGraph, m_filters));
        }
      }

      FTL::JSONObject const * varsObject = m_jsonObject->maybeGetObject( FTL_STR("vars") );
      if(varsObject)
      {
        QStringList names;
        for(FTL::JSONObject::const_iterator it = varsObject->begin();
          it != varsObject->end(); it++
          )
        {
          FTL::CStrRef key = it->first;
          names.append(key.c_str());
        }

        names.sort();
        for(int i=0;i<names.length();i++)
        {
          std::string name = names[i].toUtf8().constData();
          std::string path = m_path;
          if(path.length() != 0)
            path += ".";
          path += name;
          addChild(new VariableTreeItem(name.c_str(), path.c_str()));
        }
      }
    }
  }
  return TreeView::TreeItem::numChildren();
}

QStringList VariableGroupTreeItem::filters() const
{
  return m_filters;
}

void VariableGroupTreeItem::setFilters(QStringList f)
{
  m_filters = f;
}

bool VariableGroupTreeItem::includeChildName(QString name)
{
  QString start = name + ".";
  QString end = "." + name;

  for(int i=0;i<m_filters.length();i++)
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
