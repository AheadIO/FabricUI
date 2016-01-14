#include "BindingModelItem.h"
#include "ArgModelItem.h"
#include <assert.h>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////
BindingModelItem::BindingModelItem( FabricCore::DFGBinding binding )
  : NodeModelItem(binding.getExec(), "")
  , m_binding(binding)
{
  assert( m_binding.isValid() );
}

BaseModelItem* BindingModelItem::GetChild( QString childName )
{
  int numExisting = m_children.size();
  for (int i = 0; i < numExisting; i++)
  {
    if (m_children[i]->GetName() == childName)
      return m_children[i];
  }
  //  we shouldn't ever create a child that doesn't exist!
  assert( ChildIndex( childName ) >= 0 );
  BaseModelItem* res = new ArgModelItem( m_binding, childName );
  m_children.push_back( res );
  return res;
}

void BindingModelItem::argInserted( int index, const char * name, const char * type )
{
  m_children.insert( m_children.begin() + index,
                     new ArgModelItem( m_binding, name ) );
}
