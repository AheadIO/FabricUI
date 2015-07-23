// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "SGObjectTreeItem.h"
#include "SGObjectTreeView.h"

using namespace FabricUI;
using namespace FabricUI::SceneHub;

SGObjectTreeItem * SGObjectTreeItem::Create(
  SGObjectTreeView * view,
  FTL::StrRef name,
  FabricCore::Client * client,
  FabricCore::RTVal sgObject
  )
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

SGObjectTreeItem::SGObjectTreeItem(
  SGObjectTreeView * view,
  FTL::StrRef name,
  FabricCore::Client * client,
  FabricCore::RTVal browser
  )
  : TreeView::TreeItem( name )
{
  m_view = view;
  m_client = client;
  m_rtVal = browser;
}

SGObjectTreeItem::~SGObjectTreeItem()
{
}

unsigned int SGObjectTreeItem::numChildren()
{
  try
  {
    unsigned int count = m_rtVal.callMethod("UInt32", "getNumChildSGObjects", 0, 0).getUInt32();
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

  return TreeView::TreeItem::numChildren();
}
