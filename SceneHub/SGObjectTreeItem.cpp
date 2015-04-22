// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "SGObjectTreeItem.h"

using namespace FabricUI;
using namespace FabricUI::SceneHub;

SGObjectTreeItem * SGObjectTreeItem::Create(QString name, FabricCore::RTVal sgObject)
{
  return new SGObjectTreeItem(name, sgObject);
}

SGObjectTreeItem::SGObjectTreeItem(QString name, FabricCore::RTVal sgObject)
: TreeView::TreeItem(name)
{
  m_rtVal = sgObject;
  // call setPixmap to pick an icon

  /*
    if( (options.withProperties || options.recursive) && canAccessProperties ) {
    SGObjectPropertyIterator propIter = this.getPropertyIterator();
    SGObjectProperty property;
    while( propIter.getNext( property ) ) {
      //Careful: just getting the property type will call the generator
      if( options.callGenerators || property.isGenerated() ) {
        if( property.isReference() || property.isReferenceArray() ) {
          if( options.warnMaxLineCount( lines ) )
            return lines;

          SceneGraphDumpOptions subOptions = options;
          subOptions.maxLineCount -= lines.size();
          lines += property.dumpContent( subOptions );
        }
      }
    }
  }
  */
}

SGObjectTreeItem::~SGObjectTreeItem()
{
}

QString SGObjectTreeItem::type() const
{
  // query the rtval
  // return m_rtVal.callMethod("String", "getItemType", 0, 0).getString_cstr();
  return TreeView::TreeItem::type();
}

unsigned int SGObjectTreeItem::numChildren()
{
  // query the rtval
  // return m_rtVal.callMethod("UInt32", "getNumChildren", 0, 0).getUInt32()
  // lazily fill the contents using TreeItem::addChild

  return TreeView::TreeItem::numChildren();
}
