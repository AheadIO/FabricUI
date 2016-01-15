//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "ViewItemChildRouter.h"
#include "BaseComplexViewItem.h"

ViewItemChildRouter::ViewItemChildRouter(
  BaseComplexViewItem *viewItem,
  int index
  )
  : m_viewItem( viewItem )
  , m_index( index )
{
  setParent( viewItem );
}

void ViewItemChildRouter::connectToChild( BaseViewItem *childViewItem )
{
  connect(
    this, SIGNAL( modelValueChanged( QVariant const & ) ),
    childViewItem, SLOT( onModelValueChanged( QVariant const & ) )
    );

  connect(
    childViewItem, SIGNAL( interactionBegin() ),
    this, SLOT( onInteractionBegin() )
    );
  connect(
    childViewItem, SIGNAL( viewValueChanged( QVariant ) ),
    this, SLOT( onViewValueChanged( QVariant ) )
    );
  connect(
    childViewItem, SIGNAL( interactionEnd( bool ) ),
    this, SLOT( onInteractionEnd( bool ) )
    );
}

void ViewItemChildRouter::emitModelValueChanged( QVariant const &value )
{
  emit modelValueChanged( value );
}

void ViewItemChildRouter::onInteractionBegin()
{
  m_viewItem->onChildInteractionBegin( m_index );
}

void ViewItemChildRouter::onViewValueChanged( QVariant value )
{
  m_viewItem->onChildViewValueChanged( m_index, value );
}

void ViewItemChildRouter::onInteractionEnd( bool accept )
{
  m_viewItem->onChildInteractionEnd( m_index, accept );
}
