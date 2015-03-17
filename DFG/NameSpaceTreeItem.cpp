// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "NameSpaceTreeItem.h"
#include "PresetTreeItem.h"
#include <map>

using namespace FabricServices::DFGWrapper;
using namespace FabricUI;
using namespace FabricUI::DFG;

NameSpaceTreeItem::NameSpaceTreeItem(NameSpace nameSpace)
: TreeView::TreeItem(nameSpace.getName().c_str(), "NameSpace")
, m_nameSpace(nameSpace)
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
        lookup.insert(std::pair<std::string, NameSpace>(nameSpaces[i].getName(), nameSpaces[i]));

      for(std::map<std::string, NameSpace>::iterator it=lookup.begin();it!=lookup.end();it++)
        addChild(new NameSpaceTreeItem(it->second));
    }

    {
      std::vector<Func> funcs = m_nameSpace.getFuncs();
      std::vector<Graph> graphs = m_nameSpace.getGraphs();
      std::map<std::string, Object> lookup;
      for(size_t i=0;i<funcs.size();i++)
        lookup.insert(std::pair<std::string, Object>(funcs[i].getName(), funcs[i]));
      for(size_t i=0;i<graphs.size();i++)
        lookup.insert(std::pair<std::string, Object>(graphs[i].getName(), graphs[i]));

      for(std::map<std::string, Object>::iterator it=lookup.begin();it!=lookup.end();it++)
        addChild(new PresetTreeItem(it->second));
    }


    m_validated = true;
  }
  return TreeView::TreeItem::numChildren();
}
