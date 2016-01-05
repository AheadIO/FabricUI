#include "stdafx.h"
#include "BaseComplexViewItem.h"
#include "ViewItemChildRouter.h"

BaseComplexViewItem::BaseComplexViewItem( QString const & name )
  : BaseViewItem(name)
{
}

BaseComplexViewItem::~BaseComplexViewItem()
{
  for (int i = 0; i < m_routers.size(); i++)
    delete m_routers[i];
}

ViewItemChildRouter* BaseComplexViewItem::createChildRouter( int index )
{
  return new ViewItemChildRouter( this, index );
}

bool BaseComplexViewItem::connectChild( int index, BaseViewItem* child )
{
  if (child == NULL)
    return false;

  int oldSize = m_routers.size();
  if (index <= oldSize)
  {
    m_routers.resize( index + 1 );
    for (int i = oldSize; i < index; i++)
      m_routers[i] = NULL;
  }

  m_routers[index] = createChildRouter( index );
  m_routers[index]->connectToChild( child );
  return true;
}

bool BaseComplexViewItem::hasChildren() const 
{ 
  return true; 
}

void BaseComplexViewItem::appendChildViewItems( QList<BaseViewItem*>& items )
{
  doAppendChildViewItems( items );
  BaseViewItem::appendChildViewItems( items );
}

void BaseComplexViewItem::routeModelValueChanged( int index, const QVariant & value )
{
  if (index < m_routers.size() && m_routers[index] != NULL)
    m_routers[index]->emitModelValueChanged( value );
}
