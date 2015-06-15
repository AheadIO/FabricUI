// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "SGObjectTreeItem.h"
#include "SGObjectTreeView.h"

using namespace FabricUI;
using namespace FabricUI::SceneHub;

SGObjectTreeItem * SGObjectTreeItem::Create(SGObjectTreeView * view, QString name, FabricCore::Client * client, FabricCore::RTVal sgObject)
{
  try
  {
    FabricCore::RTVal browser = FabricCore::RTVal::Create(*client, "SGObjectHierarchyBrowser", 1, &sgObject);
    return new SGObjectTreeItem(view, name, client, browser);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
  return NULL;
}

SGObjectTreeItem::SGObjectTreeItem(SGObjectTreeView * view, QString name, FabricCore::Client * client, FabricCore::RTVal browser)
: TreeView::TreeItem(name)
{
  m_view = view;
  m_client = client;
  m_rtVal = browser;
  m_childrenCreated = false;
}

SGObjectTreeItem::~SGObjectTreeItem()
{
}

QString SGObjectTreeItem::type() const
{
  return "SGObject";
}

void SGObjectTreeItem::createChildrenIfNotDone()
{
  if( m_childrenCreated )
    return;
  try
  {
    unsigned int count = numChildren();
    if(count != TreeView::TreeItem::numChildren())
    {
      // clear previous children
      for(int i=TreeView::TreeItem::numChildren()-1;i>=0;i--)
        removeChild(child(i));

      for(unsigned int i=0;i<count;i++)
      {
        FabricCore::RTVal index = FabricCore::RTVal::ConstructUInt32(*m_client, i);
        FabricCore::RTVal browser = m_rtVal.callMethod("SGObjectHierarchyBrowser", "getChildBrowser", 1, &index);
        FabricCore::RTVal name = browser.callMethod("String", "getName", 0, 0);
        addChild(Create(m_view, name.getStringCString(), m_client, browser));
      }

      emit m_view->itemExpanded(this);
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
}

unsigned int SGObjectTreeItem::numChildren()
{
  unsigned int count = 0;
  try
  {
    count = m_rtVal.callMethod("UInt32", "getNumChildSGObjects", 0, 0).getUInt32();

    // Delay the actual creation of the children items as much as possible
    if(count != TreeView::TreeItem::numChildren())
      m_childrenCreated = false;
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }

  return count;
}


TreeView::TreeItem * SGObjectTreeItem::child( unsigned int i )
{
  // Actual creation of the children items is delayed until requested
  createChildrenIfNotDone();
  return TreeView::TreeItem::child(i);
}

TreeView::TreeItem * SGObjectTreeItem::child( QString path )
{
  // Actual creation of the children items is delayed until requested
  createChildrenIfNotDone();
  return TreeView::TreeItem::child(path);
}
