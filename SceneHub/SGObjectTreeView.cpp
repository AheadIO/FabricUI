// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "SGObjectTreeView.h"
#include <QtGui/QMenu>
#include <iostream>

using namespace FabricUI;
using namespace FabricUI::SceneHub;

SGObjectTreeView::SGObjectTreeView(QWidget * parent)
: TreeView::TreeViewWidget(parent)
{
  //std::cerr << "SGObjectTreeView::SGObjectTreeView" << std::endl;
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_lastItemClickedOn = NULL;
}

SGObjectTreeView::~SGObjectTreeView()
{
  //std::cerr << "SGObjectTreeView::~SGObjectTreeView" << std::endl;
}

void SGObjectTreeView::showCustomContextMenu(const QPoint & globalPos, TreeView::TreeItem * item)
{
  //std::cerr << "SGObjectTreeView::showCustomContextMenu" << std::endl;
  m_lastItemClickedOn = item;

  QMenu *menu=new QMenu(this);
  menu->addAction(new QAction("Expand recursively", this));
  QObject::connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(contextMenuTriggered(QAction *)));
  menu->popup(globalPos);
}

void SGObjectTreeView::contextMenuTriggered(QAction * action)
{
  std::cerr << "SGObjectTreeView::contextMenuTriggered" << std::endl;
  if(m_lastItemClickedOn == NULL)
    return;

  if(action->text() == "Expand recursively")
  {
    std::vector<TreeView::TreeItem*> items;
    items.push_back(m_lastItemClickedOn);
    std::cerr << "Nb items" << items.size() << std::endl;

    for(unsigned i=0;i<items.size();i++)
    {
      std::cerr << "Nb children" << items[i]->numChildren() << std::endl;
      for(unsigned int j=0;j<items[i]->numChildren();j++)
        items.push_back(items[i]->child(j));
    }
  }

  m_lastItemClickedOn = NULL;
}
