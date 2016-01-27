//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "BaseViewItem.h"
#include "BaseModelItem.h"
#include "ViewItemFactory.h"

#include <FabricCore.h>

#include <assert.h>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>

static int s_nInstances = 0;

BaseViewItem::BaseViewItem( QString const &name, ItemMetadata* metadata )
  : m_name( name )
  , m_metadata(metadata)
{
  s_nInstances++;
}

void BaseViewItem::setBaseModelItem( BaseModelItem* item )
{
  m_modelItem = item;

  if (m_modelItem != NULL)
  {
    QObject::connect(
      this, SIGNAL( interactionBegin() ),
      m_modelItem, SLOT( onInteractionBegin() )
      );
    QObject::connect(
      this, SIGNAL( viewValueChanged( QVariant ) ),
      m_modelItem, SLOT( onViewValueChanged( QVariant ) )
      );
    QObject::connect(
      this, SIGNAL( interactionEnd( bool ) ),
      m_modelItem, SLOT( onInteractionEnd( bool ) )
      );

    QObject::connect(
      m_modelItem, SIGNAL( modelValueChanged( QVariant const & ) ),
      this, SLOT( onModelValueChanged( QVariant const & ) )
      );

    if ( ItemMetadata *itemMetadata = m_modelItem->getMetadata() )
    {
      FTL::CStrRef vePortType =
        itemMetadata->getString( ItemMetadata::VEPortTypeKey.c_str() );
      if ( vePortType == FTL_STR("Out") )
      {
        m_metadata.setSInt32( "disabled", 1 );
      }
    }
  }
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
    return m_modelItem->getNumChildren() > 0;
  return false;
}

void BaseViewItem::appendChildViewItems( QList<BaseViewItem *>& items )
{
  if ( m_modelItem != NULL )
  {
    ViewItemFactory *viewItemFactory = ViewItemFactory::GetInstance();

    int numChildren = m_modelItem->getNumChildren();
    for ( int i = 0; i < numChildren; ++i )
    {
      BaseModelItem *childModelItem = m_modelItem->getChild( i );
      BaseViewItem* childViewItem = 
        viewItemFactory->createViewItem( childModelItem );
      if (childViewItem == NULL)
        continue;

      items.append( childViewItem );
    }
  }
}

void BaseViewItem::renameItem( QString newName )
{
  if ( m_modelItem )
    m_modelItem->rename( newName.toUtf8().constData() );
}


void BaseViewItem::onRenamed( QTreeWidgetItem* item )
{
  if (m_modelItem != NULL) 
  {
    FTL::CStrRef name = m_modelItem->getName();
    m_name = QString::fromUtf8( name.data(), name.size() );
    item->setText( 0, m_name );
  }
}

void BaseViewItem::setWidgetsOnTreeItem(
  QTreeWidget *treeWidget,
  QTreeWidgetItem *treeWidgetItem
  )
{
  treeWidgetItem->setText( 0, m_name );

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if (m_modelItem != NULL && m_modelItem->canRename())
    flags |= Qt::ItemIsEditable;

  treeWidgetItem->setFlags( flags );

  QWidget* myWidget = getWidget();

  // Disable our widgets if requested
  if (m_metadata.has( "disabled" ) &&
       m_metadata.getSInt32( "disabled" ) == 1)
  {
    myWidget->setEnabled( false );
  }

  treeWidget->setItemWidget( treeWidgetItem, 1, myWidget );
}

void BaseViewItem::metadataChanged()
{
}
