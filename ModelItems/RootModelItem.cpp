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

BaseModelItem* RootModelItem::GetChild( QString childName, bool doCreate )
{
  int numExisting = m_children.size();
  for (int i = 0; i < numExisting; i++)
  {
    if (m_children[i]->GetName() == childName)
      return m_children[i];
  }
  if (doCreate)
  {
    // Ensure this child exists, we can't assume its valid
    if (ChildIndex( childName ) >= 0)
    {
      BaseModelItem* res = CreateChild( childName );
      m_children.push_back( res );
      return res;
    }
  }
  return NULL;
}

BaseModelItem* RootModelItem::GetChild( int index, bool doCreate )
{
  QString childName = ChildName(index);
  return GetChild( childName, doCreate );
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

//////////////////////////////////////////////////////////////////////////

extern std::string FabricUI::ModelItems::SplitLast( std::string& path )
{
  size_t split = path.rfind( '.' );
  std::string res = path.substr( split + 1 );
  if (split == std::string::npos)
  {
    path.clear();
  }
  else
  {
    path = path.substr( 0, split );
  }
  return res;
}

extern std::string FabricUI::ModelItems::SplitFirst( std::string& path )
{
  size_t split = path.find( '.' );
  std::string res = path.substr( 0, split + 1 );
  if (split == std::string::npos)
  {
    path.clear();
  }
  else
  {
    path = path.substr( split, 0 );
  }
  return res;
}
