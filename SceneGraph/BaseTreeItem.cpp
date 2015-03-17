// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "BaseTreeItem.h"

using namespace FabricUI;
using namespace FabricUI::SceneGraph;

BaseTreeItem::BaseTreeItem(QString name, FabricCore::RTVal rtVal)
: TreeView::TreeItem(name)
{
  m_rtVal = rtVal;

  // call setPixmap to pick an icon
}

BaseTreeItem::~BaseTreeItem()
{
}

QString BaseTreeItem::type() const
{
  // query the rtval
  // return m_rtVal.callMethod("String", "getItemType", 0, 0).getString_cstr();
  return TreeView::TreeItem::type();
}

unsigned int BaseTreeItem::numChildren()
{
  // query the rtval
  // return m_rtVal.callMethod("UInt32", "getNumChildren", 0, 0).getUInt32()
  // lazily fill the contents using TreeItem::addChild

  return TreeView::TreeItem::numChildren();
}
