//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "VETreeWidgetItem.h"
#include "BaseViewItem.h"
#include "BaseModelItem.h"

#include <assert.h>

VETreeWidgetItem::VETreeWidgetItem( BaseViewItem *viewItem ) : m_viewItem( viewItem )
{
}

VETreeWidgetItem::~VETreeWidgetItem()
{
  // We own this pointer, we need to release it.
  if (m_viewItem != NULL)
    m_viewItem->deleteMe();
}

BaseViewItem * VETreeWidgetItem::getViewItem() const
{
  return m_viewItem;
}

bool VETreeWidgetItem::operator<( const QTreeWidgetItem &other ) const
{
  // To figure out our order, we need to ask our parent for the
  // appropriate index for this items Name vs the other items Name

  // If we have no item, we always go last
  if (m_viewItem == NULL)
    return false;
  BaseModelItem* modelItem = m_viewItem->getModelItem();
  if (modelItem == NULL)
    return false;

  const VETreeWidgetItem& otherItem =
    static_cast<const VETreeWidgetItem&>(other);
  BaseViewItem* otherView = otherItem.getViewItem();
  if (otherView == NULL)
    return true;

  VETreeWidgetItem* parentWidget = 
    static_cast<VETreeWidgetItem*>(parent());
  if (parentWidget != NULL)
  {
    BaseViewItem* parentView = parentWidget->getViewItem();
    if (parentView == NULL)
      return false;

    BaseModelItem* parentModel = parentView->getModelItem();
    if (parentModel)
    {
      int myIndex = parentModel->getChildIndex( m_viewItem->getName().toUtf8().constData() );
      int otherIndex = parentModel->getChildIndex( otherView->getName().toUtf8().constData() );

      return myIndex < otherIndex;
    }
  }
  return false;
}

