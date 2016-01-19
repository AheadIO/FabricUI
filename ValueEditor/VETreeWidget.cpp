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
#include <QtGui/QHeaderView>

#include <assert.h>
#include "../Util/QTSignalBlocker.h"

//////////////////////////////////////////////////////////////////////////

BaseModelItem* GetFirstModelItem( VETreeWidgetItem* item );
QTreeWidgetItem* FindChild( QTreeWidgetItem* parent, QString name );
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
  connect(
    this, SIGNAL( itemChanged( QTreeWidgetItem*, int ) ),
    this, SLOT( onItemEdited( QTreeWidgetItem*, int ) )
    );

  setObjectName( "valueEditor" );
  header()->close();

}


VETreeWidgetItem* VETreeWidget::createTreeWidgetItem( BaseViewItem* viewItem, QTreeWidgetItem* parent, int index /*= -1 */ )
{
  assert( viewItem );
  if (viewItem == NULL)
    return NULL;
  
  // Disable signals to prevent unnecessary 'OnEdit' signals comming back to us
  FabricUI::Util::QTSignalBlocker blocker( this );

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

    // If a child with this name already exists, overwrite it
    {
      QTreeWidgetItem* existingChild = FindChild( parent, viewItem->getName() );
      if (existingChild != NULL)
      {
        delete existingChild;
        existingChild = NULL;
      }
    }

    if (index < 0 || index >= parent->childCount())
    {
      parent->addChild( treeWidgetItem );
    }
    else
    {
      // Tab ordering will be fixed by sorting (later)
      parent->insertChild( index, treeWidgetItem );
    }
  }
  else
  {
    addTopLevelItem( treeWidgetItem );
  }

  // Add the actual widgets to the item
  viewItem->setWidgetsOnTreeItem( this, treeWidgetItem );

  // Connect a signal allowing the item to rebuild its children
  connect( viewItem, SIGNAL( rebuildChildren( BaseViewItem* ) ),
           this, SLOT( onViewItemChildrenRebuild( BaseViewItem* ) ) );

  // Ensure our columns are sized to fit our names
  resizeColumnToContents( 0 );

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

VETreeWidgetItem * VETreeWidget::findTreeWidget( BaseViewItem * pItem ) const
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

VETreeWidgetItem * VETreeWidget::findTreeWidget( BaseViewItem * pItem, VETreeWidgetItem * pWidget ) const
{
  if (pWidget == NULL)
    return NULL;

  BaseViewItem* pView = pWidget->getViewItem();
  if (pView != NULL && pView == pItem)
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
  if (parentItem != NULL)
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

void VETreeWidget::onModelItemRenamed( BaseModelItem* item )
{
  VETreeWidgetItem* treeItem = findTreeWidget( item );
  if (treeItem != NULL)
  {
    BaseViewItem *viewItem = treeItem->getViewItem();
    if (viewItem != NULL)
    {
      FabricUI::Util::QTSignalBlocker block( this );
      viewItem->onRenamed( treeItem );
    }
    // Ensure our columns are sized to fit our names
    resizeColumnToContents( 0 );
  }
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
  }
}

void VETreeWidget::onViewItemChildrenRebuild( BaseViewItem* item )
{
  VETreeWidgetItem* widget = findTreeWidget( item );
  if (widget != NULL)
  {
    if (widget->isExpanded())
    {
      onTreeWidgetItemCollapsed( widget );
      onTreeWidgetItemExpanded( widget );
    }
  }
}

void VETreeWidget::onModelItemChildrenReordered( BaseModelItem* /*parent*/, const QList<int>& /*newOrder*/ )
{
  sortItems( 0, Qt::AscendingOrder );
}

void VETreeWidget::onItemEdited( QTreeWidgetItem* _item, int column )
{
  // We only deal with name changes here
  if (column != 0)
    return;

  VETreeWidgetItem* item = static_cast<VETreeWidgetItem*>(_item);
  if (item != NULL)
  {
    BaseViewItem* viewItem = item->getViewItem();
    if (viewItem != NULL)
    {
      viewItem->renameItem( item->text( 0 ) );

      //FabricUI::Util::QTSignalBlocker(this);
      //item->setText( 0, viewItem->getName() );
    }
  }
}

void VETreeWidget::onSetModelItem( BaseModelItem* pItem )
{
  ViewItemFactory* pFactory = ViewItemFactory::GetInstance();
  BaseViewItem* pViewLayer = pFactory->CreateViewItem( pItem );
  // Remove all existing
  clear();
  VETreeWidgetItem* newItem = createTreeWidgetItem( pViewLayer, NULL );
  if (newItem != NULL)
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
  if (model == NULL)
    return;

  QAction *newAct = new QAction( tr( "Reset to Default" ), this );
  newAct->setStatusTip( tr( "Reset this ports current value to the default" ) );

  newAct->setEnabled( model->hasDefault() );
  connect( newAct, SIGNAL( triggered() ), this, SLOT( resetItem() ) );

  QMenu menu( this );
  menu.addAction( newAct );
  menu.exec( mapToGlobal( pt ) );
}

void VETreeWidget::resetItem()
{
  QList<QTreeWidgetItem*> items = selectedItems();
  if (items.empty())
    items.push_back( currentItem() );

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

QTreeWidgetItem* FindChild( QTreeWidgetItem* parent, QString name )
{
  for (int i = 0; i < parent->childCount(); i++)
  {
    QTreeWidgetItem* item = parent->child( i );
    if (item && item->text( 0 ) == name)
      return item;
  }
  return NULL;
}

QTreeWidgetItem* FindOutNode( QTreeWidgetItem* parent )
{
  if (parent == NULL)
    return NULL;

  if (parent->text(0) == s_outNodeName)
    return parent;

  return FindChild( parent, s_outNodeName );
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
