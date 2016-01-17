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
  // Ensure this child exists, we can't assume its valid
  if (ChildIndex( childName ) >= 0)
  {
    BaseModelItem* res = CreateChild( childName );
    m_children.push_back( res );
    return res;
  }
  return NULL;
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

bool RootModelItem::argInserted( int index, const char* name, const char* type )
{
  // Assert that this child exists.  We will get notifications
  // for children that we do not have.
  if (ChildName( index ) == name)
  {
    return true;
  }
  return false;
}

bool RootModelItem::argTypeChanged( int index, const char* name, const char* newType )
{
  // Assert that this child exists.  We will get notifications
  // for children that we do not have.
  if (ChildName( index ) == name)
  {
    return true;
  }
  return false;
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


BaseModelItem* FabricUI::ModelItems::RootModelItem::argRenamed( const char* oldName, const char* newName )
{
  for (ChildVec::iterator itr = m_children.begin(); itr != m_children.end(); itr++)
  {
    if ((*itr)->GetName() == oldName)
    {
      (*itr)->OnItemRenamed( newName );
      return *itr;
    }
  }
  return NULL;
}

bool FabricUI::ModelItems::RootModelItem::hasDefault()
{
  return false;
}

void FabricUI::ModelItems::RootModelItem::resetToDefault()
{
}
