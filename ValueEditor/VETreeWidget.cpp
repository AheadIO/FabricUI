//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "VETreeWidget.h"
#include "VETreeWidgetItem.h"
#include "BaseViewItem.h"
#include "BaseModelItem.h"
#include "ViewItemFactory.h"

#include <FabricCore.h>
#include <FTL/Path.h>
#include <QtCore/QFile>
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

  setObjectName( "ValueEditor" );
  header()->close();

  reloadStyles();
}

void VETreeWidget::reloadStyles()
{
  char const *fabricDir = getenv( "FABRIC_DIR" );
  if ( !fabricDir || !fabricDir[0] )
  {
    fprintf(
      stderr,
      "VETreeWidget::reloadStyles: missing FABRIC_DIR environment variable\n"
      );
    return;
  }

  std::string qssPath = fabricDir;
  FTL::PathAppendEntry( qssPath, "Resources" );
  FTL::PathAppendEntry( qssPath, "QSS" );
  FTL::PathAppendEntry( qssPath, "ValueEditor.qss" );

#if defined(FTL_PLATFORM_WINDOWS)
  QFile qssFile( QString::fromLatin1( qssPath.data(), qssPath.size() ) );
#else
  QFile qssFile( QString::fromUtf8( qssPath.data(), qssPath.size() ) );
#endif
  if ( !qssFile.open( QFile::ReadOnly ) )
  {
    fprintf(
      stderr,
      "VETreeWidget::reloadStyles: unable to open '%s'\n",
      qssPath.c_str()
      );
    return;
  }

  QString styleSheet = QLatin1String( qssFile.readAll() );
  if ( styleSheet.isEmpty() )
  {
    fprintf(
      stderr,
      "VETreeWidget::reloadStyles: style sheet '%s' is empty\n",
      qssPath.c_str()
      );
    return;
  }

  setStyleSheet( styleSheet );
}

// Perform a depth-first gather of the child widgets of this item
void gatherTabWidgets(
  QObject *item,
  QWidget *matchingWindow,
  QWidgetList &tabWidgets
  )
{
  if (item->isWidgetType())
  {
    QWidget* itemWidget = static_cast<QWidget*>( item );
    if ( itemWidget->window() == matchingWindow
      && ( itemWidget->focusPolicy() & Qt::TabFocus ) )
      tabWidgets.push_back( itemWidget );
  }
  const QObjectList& children = item->children();
  for (QObjectList::const_iterator itr = children.begin(); itr != children.end(); itr++)
  {
    gatherTabWidgets(
      const_cast<QObject*>(*itr),
      matchingWindow,
      tabWidgets
      );
  }
}

// Perform a depth-first gather of the child items widgets;
void gatherTabWidgets(
  VETreeWidgetItem* item,
  QWidget *matchingWindow,
  QWidgetList &tabWidgets
  )
{
  BaseViewItem* itemView = item->getViewItem();
  QString itemName = item->text( 0 );
  if (itemView != NULL)
  {
    QWidget* itemWidget = itemView->getWidget();
    gatherTabWidgets( itemWidget, matchingWindow, tabWidgets );
  }

  for (int i = 0; i < item->childCount(); i++)
  {
    VETreeWidgetItem* child =
      static_cast<VETreeWidgetItem*>(item->child( i ));
    gatherTabWidgets( child, matchingWindow, tabWidgets );
  }
}

void VETreeWidget::sortTree()
{
  // Ensure ordering is maintained.
  sortItems( 0, Qt::AscendingOrder );

  // Once the items have been re-ordered, re-create the tab ordering
  QWidget *myWindow = window();
  QWidgetList tabWidgets;
  for ( int i = 0; i < topLevelItemCount(); ++i )
  {
    VETreeWidgetItem* baseItem = static_cast<VETreeWidgetItem*>(topLevelItem( 0 ));
    gatherTabWidgets( baseItem, myWindow, tabWidgets );
  }

  if (!tabWidgets.empty())
  {
    QWidget* lastWidget = tabWidgets.first();
    // Connect this treeview to the first widget.  This ensures
    // if the treeview is tabbed-to, it will tab to the correct sub-widget
    setTabOrder( this, lastWidget );
    for (QWidgetList::iterator itr = tabWidgets.begin() + 1; itr != tabWidgets.end(); itr++)
    {
      setTabOrder( lastWidget, *itr );
      lastWidget = *itr;
    }
  }
}

// Get the next widget in the focus chain that accepts tab focus
QWidget* getNextTabWidget( QWidget* qw )
{
  assert( qw != NULL );
  do {
    qw = qw->nextInFocusChain();
  } while (qw != NULL && !(qw->focusPolicy() | Qt::TabFocus));
    
  return qw;
}

// Get the previous widget in the focus chain that accepts tab focus
QWidget* getPrevTabWidget( QWidget* qw )
{
  assert( qw != NULL );
  do {
    qw = qw->previousInFocusChain();
  } while (qw != NULL && !(qw->focusPolicy() | Qt::TabFocus));

  return qw;
}

// Returns true if the passed widget is an acceptable child
// widget for accepting Tab's.  This function should only return
// true for widgets that are set for VETreeWidgetItems
bool isValidTabChild( VETreeWidget* tree, QWidget* qw )
{
  if (qw == NULL)
    return false;

  // The structure of the QTreeWidget can be a little bit confusing
  // The widgets on the tree are not immediate descendants - instead
  // they are all children of an intermediate scroll_area class.
  // We do not want to tab to the scroll area, so we filter by:
  // qw != tree : obviously we do not want to tab to the tree itself
  // isAncestorOf : Only children of the tree are accepted
  // parentWidget != tree : Do not accept immediate descendants.  (This assumes
  // the internal structure of QTreeView has an intermediate class)
  return qw != tree && tree->isAncestorOf( qw ) && qw->parentWidget() != tree;
}

// We override the default focus handler to provide looping
// Qt cannot specify tab loops (due to the way setTabOrder is implemented).
// In this function we detect if the tab would move focus away from 
// this tree-views widgets, and reset the focus back to either the
// start or end of the tree's focus chain.
bool VETreeWidget::focusNextPrevChild( bool next )
{
  QWidget* currentFocus = focusWidget();
  if (currentFocus != NULL)
  {
    if (next)
    {
      // Check if the widget focus is moving to is one of our children
      QWidget* nextFocus = getNextTabWidget( currentFocus );
      if (!isValidTabChild( this, nextFocus ))
      {
        // Prevent focus from moving out of the Tree by looping back to start
        QWidget* myNextFocus = nextInFocusChain();
        myNextFocus->setFocus( Qt::TabFocusReason );
        return true;
      }
    }
    else
    {
      // Check if the widget focus is moving to is one of our children
      QWidget* prevFocus = getPrevTabWidget( currentFocus );
      if (!isValidTabChild( this, prevFocus ))
      {
        // Focus to the last child in the chain...
        // Unfortunately there is no simple & predictable way to do this.
        // To do this we iterate through the whole chain until
        // we reach the end of this tree's widgets, then back up one
        QWidget* myLastFocus = currentFocus;
        QWidget* myLastNextFocus = getNextTabWidget( currentFocus );
        while (isValidTabChild( this, myLastNextFocus ))
        {
          myLastFocus = myLastNextFocus;
          myLastNextFocus = getNextTabWidget( myLastFocus );
        }
        myLastFocus->setFocus( Qt::TabFocusReason );
        return true;
      }
    }
  }
  // If we are not moving focus out of the tree, then simply use default handler.
  return QTreeWidget::focusNextPrevChild( next );
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
    BaseModelItem* modelItem = viewItem->getModelItem();
    if (modelItem != NULL)
    {
      if ( ItemMetadata *itemMetadata = modelItem->getMetadata() )
      {
        FTL::CStrRef vePortType =
          itemMetadata->getString( ItemMetadata::VEPortTypeKey.c_str() );
        if ( vePortType == FTL_STR("Out") )
        {
          parent = GetOutNode( parent );
        }
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

VETreeWidgetItem* VETreeWidget::findTreeWidget( QWidget* widget ) const
{
  for (int i = 0; i < topLevelItemCount(); i++)
  {
    VETreeWidgetItem* widgetItem =
      static_cast<VETreeWidgetItem*>(topLevelItem( i ));

    if (widgetItem != NULL)
    {
      VETreeWidgetItem* match = findTreeWidget( widget, widgetItem );
      if (match != NULL)
        return match;
    }
  }
  return NULL;
}

VETreeWidgetItem * VETreeWidget::findTreeWidget( QWidget* widget, VETreeWidgetItem * item ) const
{
  if (item == NULL)
    return NULL;

  for (int i = 0; i < columnCount(); i++)
  {
    if (itemWidget( item, i ) == widget)
      return item;
  }

  for (int i = 0; i < item->childCount(); i++)
  {
    VETreeWidgetItem* pMatch = findTreeWidget(
      widget,
      static_cast<VETreeWidgetItem*>(item->child( i )) );

    if (pMatch != NULL)
      return pMatch;
  }
  return NULL;
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
  if (pView != NULL && pView->getModelItem() == pItem)
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


void VETreeWidget::onModelItemChildInserted(
  BaseModelItem* parent,
  int index,
  const char* name
  )
{
  QTreeWidgetItem* parentItem = findTreeWidget( parent );
  if (parentItem != NULL)
  {
    parentItem->setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
    if (parentItem->isExpanded())
    {
      // Insert new child in the appropriate place
      BaseModelItem* newItem = parent->getChild( name );
      BaseViewItem* newView =
        ViewItemFactory::GetInstance()->createViewItem( newItem );
      createTreeWidgetItem( newView, parentItem, index );

      sortTree();
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
        ViewItemFactory::GetInstance()->createViewItem( item );
      createTreeWidgetItem( newView, parentItem, index );

      sortTree();
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
  sortTree();
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
  BaseViewItem* pViewLayer = pFactory->createViewItem( pItem );
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
    BaseViewItem* lastViewItem = viewItem;
    for (int i = 0; i < childViewItems.size(); ++i)
    {
      BaseViewItem *childViewItem = childViewItems.at( i );
      createTreeWidgetItem( childViewItem, treeWidgetItem );
      setTabOrder( lastViewItem->getWidget(), childViewItem->getWidget() );
      lastViewItem = childViewItem;
    }
    sortTree();
  }
  else
    this->resizeColumnToContents( 0 );
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
  this->resizeColumnToContents( 0 );
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
    BaseModelItem* pModelItem = pViewItem->getModelItem();
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
