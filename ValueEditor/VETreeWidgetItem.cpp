#include "stdafx.h"
#include "VETreeWidgetItem.h"
#include "BaseViewItem.h"
#include "BaseModelItem.h"

VETreeWidgetItem::VETreeWidgetItem( BaseViewItem *viewItem ) : m_viewItem( viewItem )
{
  assert( viewItem );
}

VETreeWidgetItem::~VETreeWidgetItem()
{
  // We own this pointer, we need to release it.
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

  if (m_viewItem == NULL)
    return false;
  BaseModelItem* modelItem = m_viewItem->GetModelItem();
  if (modelItem == NULL)
    return false;

  const VETreeWidgetItem& otherItem =
    static_cast<const VETreeWidgetItem&>(other);
  BaseViewItem* otherView = otherItem.getViewItem();
  if (otherView == NULL)
    return false;

  VETreeWidgetItem* parentWidget = 
    static_cast<VETreeWidgetItem*>(parent());
  if (parentWidget != NULL)
  {
    BaseViewItem* parentView = parentWidget->getViewItem();
    if (parentView == NULL)
      return false;

    BaseModelItem* parentModel = parentView->GetModelItem();
    if (parentModel)
    {
      int myIndex = parentModel->ChildIndex( m_viewItem->getName() );
      int otherIndex = parentModel->ChildIndex( otherView->getName() );
      return myIndex < otherIndex;
    }
  }
  return false;
}

