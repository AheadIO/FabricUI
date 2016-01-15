#include "RootModelItem.h"
#include <assert.h>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////
RootModelItem::RootModelItem()
{

}


RootModelItem::~RootModelItem()
{

}

BaseModelItem* RootModelItem::GetChild( QString childName )
{
  int numExisting = m_children.size();
  for (int i = 0; i < numExisting; i++)
  {
    if (m_children[i]->GetName() == childName)
      return m_children[i];
  }
  //  we shouldn't ever create a child that doesn't exist!
  assert( ChildIndex( childName ) >= 0 );
  BaseModelItem* res = CreateChild( childName );
  m_children.push_back( res );
  return res;
}

BaseModelItem* RootModelItem::GetChild( int index )
{
  QString childName = ChildName(index);
  return GetChild( childName );
}

int RootModelItem::ChildIndex( QString name )
{
  int numChildren = NumChildren();
  for (int i = 0; i < numChildren; i++)
  {
    if (ChildName( i ) == name)
    {
      return i;
    }
  }
  return -1;
}


void RootModelItem::argInserted( int index, const char* name, const char* type )
{
  m_children.insert( m_children.begin() + index, CreateChild( name ) );
}

void RootModelItem::argTypeChanged( int index, const char* name, const char* newType )
{
  BaseModelItem* pChild = GetChild( name );
  assert( pChild != NULL );
  if (pChild != NULL)
  {
    assert( pChild->GetName() == name );
    // TODO:  What?  Reset the QVariant?
  }
}

void RootModelItem::argRemoved( int index, const char* name )
{
  for (ChildVec::iterator itr = m_children.begin(); itr != m_children.end(); itr++)
  {
    if ((*itr)->GetName() == name)
    {
      delete *itr;
      m_children.erase( itr );
      break;
    }
  }
}

bool FabricUI::ModelItems::RootModelItem::hasDefault()
{
  return false;
}

void FabricUI::ModelItems::RootModelItem::resetToDefault()
{
}
