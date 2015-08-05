// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "SGObjectTreeModel.h"
#include "SGObjectTreeItem.h"
#include <iostream>

using namespace FabricUI;
using namespace FabricUI::SceneHub;

SGObjectTreeModel::SGObjectTreeModel(QObject * parent)
:TreeView::TreeModel(parent)
{
  //std::cerr << "SGObjectTreeModel::SGObjectTreeModel" << std::endl;
}

SGObjectTreeModel::~SGObjectTreeModel()
{
  //std::cerr << "SGObjectTreeModel::~SGObjectTreeModel" << std::endl;
}

void SGObjectTreeModel::onSceneHierarchyChanged()
{
  //std::cerr << "TreeModel::hierarchyChanged" << std::endl;

  std::vector<SGObjectTreeItem*> roots;
  for( unsigned int i = 0; i < numItems(); ++i ) {
    SGObjectTreeItem * sgItem = (SGObjectTreeItem*)item(i);
    if( sgItem && sgItem->parent() == NULL )
      roots.push_back(sgItem);
  }
  for( unsigned int i = 0; i < roots.size(); ++i )
    roots[i]->updateRecursively();
}
