// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "SGObjectTreeView.h"
#include <QtGui/QMenu>

using namespace FabricUI;
using namespace FabricUI::SceneHub;

SGObjectTreeView::SGObjectTreeView(QWidget * parent)
: TreeView::TreeViewWidget(parent)
{
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_lastItemClickedOn = NULL;
}

SGObjectTreeView::~SGObjectTreeView()
{
}

void SGObjectTreeView::showCustomContextMenu(const QPoint & globalPos, TreeView::TreeItem * item)
{
  m_lastItemClickedOn = item;

  QMenu *menu=new QMenu(this);
  menu->addAction(new QAction("Expand recursively", this));
  QObject::connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(contextMenuTriggered(QAction *)));
  menu->popup(globalPos);
}

void SGObjectTreeView::contextMenuTriggered(QAction * action)
{
  if(m_lastItemClickedOn == NULL)
    return;

  if(action->text() == "Expand recursively")
  {
    std::vector<TreeView::TreeItem*> items;
    items.push_back(m_lastItemClickedOn);
    for(unsigned i=0;i<items.size();i++)
    {
      for(unsigned int j=0;j<items[i]->numChildren();j++)
        items.push_back(items[i]->child(j));
    }

    emit itemRecursivelyExpanded(m_lastItemClickedOn);
  }

  m_lastItemClickedOn = NULL;
}
