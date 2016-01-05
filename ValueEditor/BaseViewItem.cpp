#include "stdafx.h"
#include "BaseViewItem.h"
#include "BaseModelItem.h"
#include "ViewItemFactory.h"


static int s_nInstances = 0;
BaseViewItem::BaseViewItem( QString const &name )
  : m_name( name )
{
  s_nInstances++;
}


BaseViewItem::~BaseViewItem()
{
  s_nInstances--;
}

// El-cheapo mem leak detection.
// A static instance of this class verifies on 
// Shutdown that there are no leaked entities
static class Counter {
public: ~Counter() { assert( s_nInstances == 0 ); }
} InstCounter;

int BaseViewItem::numInstances() 
{
  return s_nInstances;
}

bool BaseViewItem::hasChildren() const
{
  if ( !!m_modelItem )
    return m_modelItem->NumChildren() > 0;
  return false;
}

void BaseViewItem::appendChildViewItems( QList<BaseViewItem *>& items )
{
  if ( m_modelItem != NULL )
  {
    ViewItemFactory *viewItemFactory = ViewItemFactory::GetInstance();

    size_t numChildren = m_modelItem->NumChildren();
    for (int i = 0; i < numChildren; ++i)
    {
      BaseModelItem *childModelItem = m_modelItem->GetChild( i );
      BaseViewItem* childViewItem = viewItemFactory->BuildView( childModelItem );
      if (childViewItem == NULL)
        continue;

      items.append( childViewItem );
    }
  }
}

void BaseViewItem::setWidgetsOnTreeItem(
  QTreeWidget *treeWidget,
  QTreeWidgetItem *treeWidgetItem
  )
{
  treeWidgetItem->setText( 0, m_name );
  treeWidget->setItemWidget( treeWidgetItem, 1, getWidget() );
}

// Include MOC'ed file here, in order
// to support PCH on windows.
#include "moc_BaseViewItem.cpp"