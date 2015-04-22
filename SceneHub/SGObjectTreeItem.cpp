// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "SGObjectTreeItem.h"

using namespace FabricUI;
using namespace FabricUI::SceneHub;

SGObjectTreeItem * SGObjectTreeItem::Create(QString name, FabricCore::Client * client, FabricCore::RTVal sgObject)
{
  try
  {
    FabricCore::RTVal browser = FabricCore::RTVal::Create(*client, "SGObjectHierarchyBrowser", 1, &sgObject);
    return new SGObjectTreeItem(name, client, browser);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
  return NULL;
}

SGObjectTreeItem::SGObjectTreeItem(QString name, FabricCore::Client * client, FabricCore::RTVal browser)
: TreeView::TreeItem(name)
{
  m_client = client;
  m_rtVal = browser;
}

SGObjectTreeItem::~SGObjectTreeItem()
{
}

QString SGObjectTreeItem::type() const
{
  return "SGObject";
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
        FabricCore::RTVal sgObject = m_rtVal.callMethod("SGObject", "getChildSGObject", 1, &index);
        FabricCore::RTVal name = sgObject.callMethod("String", "getName", 0, 0);
        addChild(Create(name.getStringCString(), m_client, sgObject));
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }

  return TreeView::TreeItem::numChildren();
}
