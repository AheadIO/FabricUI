//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "VETreeWidget.h"
#include "VETreeWidgetItem.h"
#include "BaseViewItem.h"
#include "BaseModelItem.h"
#include "ViewItemFactory.h"

#include <FabricCore.h>
#include <QtGui/QAction>
#include <QtGui/QMenu>

#include <assert.h>

//////////////////////////////////////////////////////////////////////////

BaseModelItem* GetFirstModelItem( VETreeWidgetItem* item );
QTreeWidgetItem* FindOutNode( QTreeWidgetItem* parent );
QTreeWidgetItem* GetOutNode( QTreeWidgetItem* parent );

const QString s_outNodeName = " - Out - ";

//////////////////////////////////////////////////////////////////////////

VETreeWidget::VETreeWidget( )
{
  setColumnCount( 2 );
  setContextMenuPolicy( Qt::CustomContextMenu );
  setSelectionMode( NoSelection );

  connect(
    this, SIGNAL( itemExpanded( QTreeWidgetItem * ) ),
    this, SLOT( onTreeWidgetItemExpanded( QTreeWidgetItem * ) )
    );
  connect(
    this, SIGNAL( itemCollapsed( QTreeWidgetItem * ) ),
    this, SLOT( onTreeWidgetItemCollapsed( QTreeWidgetItem * ) )
    );
  connect( 
    this, SIGNAL( customContextMenuRequested( const QPoint& ) ),
    this, SLOT( prepareMenu( const QPoint& ) )
    );

  setObjectName( "valueEditor" );

}


VETreeWidgetItem* VETreeWidget::createTreeWidgetItem( BaseViewItem* viewItem, QTreeWidgetItem* parent, int index /*= -1 */ )
{
  assert( viewItem );
  if (viewItem == NULL)
    return NULL;

  VETreeWidgetItem *treeWidgetItem = new VETreeWidgetItem( viewItem );
  if (viewItem->hasChildren())
    treeWidgetItem->setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
  else
    treeWidgetItem->setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicator );

  if (parent)
  {
    BaseModelItem* modelItem = viewItem->GetModelItem();
    if (modelItem != NULL)
    {
      if (modelItem->GetInOut() == FabricCore::DFGPortType_Out)
      {
        parent = GetOutNode( parent );
      }
    }
    if (index < 0 || index >= parent->childCount())
      parent->addChild( treeWidgetItem );
    else
      parent->insertChild( index, treeWidgetItem );
  }
  else
  {
    addTopLevelItem( treeWidgetItem );
  }

  viewItem->setWidgetsOnTreeItem( this, treeWidgetItem );
  return treeWidgetItem;
}

VETreeWidgetItem * VETreeWidget::findTreeWidget( BaseModelItem * pItem ) const
{
  for (int i = 0; i < topLevelItemCount(); i++)
  {
    VETreeWidgetItem* widgetItem = 
      static_cast<VETreeWidgetItem*>(topLevelItem( i ));

    if (widgetItem != NULL)
    {
      VETreeWidgetItem* pMatch = findTreeWidget( pItem, widgetItem );
      if (pMatch != NULL)
        return pMatch;
    }
  }
  return NULL;
}

VETreeWidgetItem * VETreeWidget::findTreeWidget( BaseModelItem * pItem, VETreeWidgetItem * pWidget ) const
{
  if (pWidget == NULL)
    return NULL;

  BaseViewItem* pView = pWidget->getViewItem();
  if (pView != NULL && pView->GetModelItem() == pItem)
    return pWidget;
  
  for (int i = 0; i < pWidget->childCount(); i++)
  {
    VETreeWidgetItem* pMatch = findTreeWidget(
      pItem,
      static_cast<VETreeWidgetItem*>(pWidget->child( i )) );

    if (pMatch != NULL)
      return pMatch;
  }
  return NULL;
}

void VETreeWidget::onModelItemChildInserted( BaseModelItem* parent, int index, const char* name )
{
  QTreeWidgetItem* parentItem = findTreeWidget( parent );
  if (parent != NULL)
  {
    parentItem->setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
    if (parentItem->isExpanded())
    {
      // Insert new child in the appropriate place
      BaseModelItem* newItem = parent->GetChild( name );
      BaseViewItem* newView =
        ViewItemFactory::GetInstance()->CreateViewItem( newItem );
      createTreeWidgetItem( newView, parentItem, index );

      // Ensure ordering is maintained.
      sortItems( 0, Qt::AscendingOrder );
    }
  }
}

void VETreeWidget::onModelItemRemoved( BaseModelItem* item )
{
  VETreeWidgetItem* oldWidget = findTreeWidget( item );
  if (oldWidget != NULL)
    delete oldWidget;
}

void VETreeWidget::onModelItemTypeChanged( BaseModelItem* item, const char* /*newType*/ )
{
  VETreeWidgetItem* oldWidget = findTreeWidget( item );
  if (oldWidget != NULL)
  {
    // We can get repeat messages for type changed, check
    // that its not actually the same type.  If we start
    // getting slow, checking type here would be a sensible
    // operation.
    //if (item->GetValue() == oldWidget->getViewItem()->)

    QTreeWidgetItem* parentItem = oldWidget->parent();
    if (parentItem != NULL)
    {
      int index = parentItem->indexOfChild( oldWidget );
      BaseViewItem* newView =
        ViewItemFactory::GetInstance()->CreateViewItem( item );
      createTreeWidgetItem( newView, parentItem, index );
    }
    delete oldWidget;
  }
}

void VETreeWidget::onModelItemChildrenReordered( BaseModelItem* /*parent*/, const QList<int>& /*newOrder*/ )
{
  sortItems( 0, Qt::AscendingOrder );
}

void VETreeWidget::onSetModelItem( BaseModelItem* pItem )
{
  ViewItemFactory* pFactory = ViewItemFactory::GetInstance();
  BaseViewItem* pViewLayer = pFactory->CreateViewItem( pItem );
  // Remove all existing
  clear();
  VETreeWidgetItem* newItem = createTreeWidgetItem( pViewLayer, NULL );
  newItem->setExpanded( true );

}

void VETreeWidget::onTreeWidgetItemExpanded( QTreeWidgetItem *_treeWidgetItem )
{
  VETreeWidgetItem *treeWidgetItem =
    static_cast<VETreeWidgetItem *>( _treeWidgetItem );
  BaseViewItem *viewItem = treeWidgetItem->getViewItem();

  if (viewItem != NULL)
  {
    QList<BaseViewItem *> childViewItems;
    viewItem->appendChildViewItems( childViewItems );
    for (int i = 0; i < childViewItems.size(); ++i)
    {
      BaseViewItem *childViewItem = childViewItems.at( i );
      createTreeWidgetItem( childViewItem, treeWidgetItem );
    }
    sortItems( 0, Qt::AscendingOrder );
  }
}

void VETreeWidget::onTreeWidgetItemCollapsed( QTreeWidgetItem *_treeWidgetItem )
{
  VETreeWidgetItem *treeWidgetItem =
    static_cast<VETreeWidgetItem *>(_treeWidgetItem);
  BaseViewItem *viewItem = treeWidgetItem->getViewItem();

  if (viewItem != NULL)
  {
    for (int i = treeWidgetItem->childCount(); i--; )
    {
      VETreeWidgetItem *childTreeWidgetItem =
        static_cast<VETreeWidgetItem *>(treeWidgetItem->child( i ));
      treeWidgetItem->removeChild( childTreeWidgetItem );
      delete childTreeWidgetItem;
    }
  }
}

void VETreeWidget::prepareMenu( const QPoint& pt )
{
  VETreeWidgetItem* item = static_cast<VETreeWidgetItem*>(itemAt( pt ));
  BaseModelItem* model = GetFirstModelItem( item );

  QAction *newAct = new QAction( tr( "Reset" ), this );
  newAct->setStatusTip( tr( "new sth" ) );

  newAct->setEnabled( model->hasDefault() );
  connect( newAct, SIGNAL( triggered() ), this, SLOT( resetItem() ) );

  QMenu menu( this );
  menu.addAction( newAct );
  menu.exec( mapToGlobal( pt ) );
}

void VETreeWidget::resetItem()
{
  QList<QTreeWidgetItem*> items = selectedItems();
  for (int i = 0; i < items.size(); i++)
  {
    VETreeWidgetItem* item = static_cast<VETreeWidgetItem*>(items[i]);
    BaseModelItem* model = GetFirstModelItem( item );
    if (model)
      model->resetToDefault();
  }
}

//////////////////////////////////////////////////////////////////////////

BaseModelItem* GetFirstModelItem( VETreeWidgetItem* item )
{
  if (item == NULL)
    return NULL;

  BaseViewItem* pViewItem = item->getViewItem();
  if (pViewItem != NULL)
  {
    BaseModelItem* pModelItem = pViewItem->GetModelItem();
    if (pModelItem != NULL)
      return pModelItem;
  }
  VETreeWidgetItem* parent =
    static_cast<VETreeWidgetItem*>(item->parent());

  return GetFirstModelItem( parent );
}

QTreeWidgetItem* FindOutNode( QTreeWidgetItem* parent )
{
  if (parent == NULL)
    return NULL;

  if (parent->text(0) == s_outNodeName)
    return parent;

  for (int i = 0; i < parent->childCount(); i++)
  {
    QTreeWidgetItem* child = parent->child( i );
    if (child->text(0) == s_outNodeName)
      return child;
  }
  return NULL;
}

QTreeWidgetItem* GetOutNode( QTreeWidgetItem* parent )
{
  // Out values get moved into their own sub-item
  QTreeWidgetItem* outNode = FindOutNode( parent );
  if (outNode == NULL)
  {
    outNode = new VETreeWidgetItem( NULL );
    outNode->setText( 0, s_outNodeName );
    outNode->setFirstColumnSpanned( true );
    parent->addChild( outNode );
  }
  return outNode;
}
