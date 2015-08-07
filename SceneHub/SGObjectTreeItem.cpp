// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
/*
#include "SGObjectTreeItem.h"
#include "SGObjectTreeView.h"
#include "SGObjectTreeModel.h"
#include <iostream>

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

FabricCore::RTVal SGObjectTreeItem::getSGObject() {
 FabricCore::RTVal val;
  try
  {
   val = m_rtVal.callMethod("SGObject", "getObject", 0, 0);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
 return val;
}

void SGObjectTreeItem::updateChildrenCount() {

  try {
    FabricCore::RTVal changedResults[2];
    changedResults[0] = FabricCore::RTVal::ConstructBoolean( *m_client, false );
    changedResults[1] = FabricCore::RTVal::ConstructBoolean( *m_client, false );

    FabricCore::RTVal browser = m_rtVal.callMethod( "SGObjectHierarchyBrowser", "update", 2, changedResults );
    bool sceneHierarchyChanged = changedResults[1].getBoolean();
    if( !sceneHierarchyChanged )
      return;

    bool changed = changedResults[0].getBoolean();
    if( !changed )
      return;

    // clear previous children.
    // TODO: remap / keep those that didn't change...
    removeChildrenRecursively();

    m_childrenCreated = false;
    emit ((SGObjectTreeModel*)model())->itemChanged(this);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
}

// Creating the children will access the reference targets which will trigger childrens' actual creation
// if these are generated.
void SGObjectTreeItem::createChildrenIfNotDone()
{
  if( m_childrenCreated )
    return;
  try
  {
    unsigned int count = numChildren();

    for(unsigned int i=0;i<count;i++)
    {
      FabricCore::RTVal index = FabricCore::RTVal::ConstructUInt32(*m_client, i);
      FabricCore::RTVal browser = m_rtVal.callMethod("SGObjectHierarchyBrowser", "getChildBrowser", 1, &index);
      FabricCore::RTVal name = browser.callMethod("String", "getName", 0, 0);
      //std::cerr << "name[" << i << "] " << name.getStringCString() << std::endl;
      SceneHub::SGObjectTreeItem * item = Create(m_view, name.getStringCString(), m_client, browser);
      addChild(item);
    }
    m_childrenCreated = true;
    emit ((SGObjectTreeModel*)model())->itemChanged(this);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
}

unsigned int SGObjectTreeItem::numChildren() {
  unsigned int count = 0;
  try
  {
    updateChildrenCount();
    count = m_rtVal.callMethod("UInt32", "getNumChildSGObjects", 0, 0).getUInt32();
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
  return count;
}

void SGObjectTreeItem::updateRecursively() {
  if( !m_childrenCreated )
    return; // We want to update existing items, not expand to new ones

  updateChildrenCount();
  createChildrenIfNotDone();

  unsigned int count = numChildren();
  for( unsigned int i = 0; i < count; ++i ) {
    SGObjectTreeItem * sgChild = (SGObjectTreeItem *)child( i );
    if( sgChild )
      sgChild->updateRecursively();
  }
}

void SGObjectTreeItem::loadRecursively() {
  try
  {
    FabricCore::RTVal sgObject = getSGObject();
    sgObject.callMethod("", "forceHierarchyExpansion", 0, 0);

    emit ((SGObjectTreeModel*)model())->itemChanged(this);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
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
*/
