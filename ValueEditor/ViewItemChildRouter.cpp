#include "stdafx.h"
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
    childViewItem, SIGNAL( viewValueChanged( QVariant const &, bool ) ),
    this, SLOT( onViewValueChanged( QVariant const &, bool ) )
    );
}

void ViewItemChildRouter::emitModelValueChanged( QVariant const &value )
{
  emit modelValueChanged( value );
}

void ViewItemChildRouter::onViewValueChanged( QVariant const &value, bool commit )
{
  m_viewItem->onChildViewValueChanged( m_index, value, commit );
}

// Include MOC'ed file here, in order
// to support PCH on windows.
#include "moc_ViewItemChildRouter.cpp"