//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/RootModelItem.h>
#include <assert.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
RootModelItem::RootModelItem()
{

}


RootModelItem::~RootModelItem()
{
  for ( ChildVec::iterator it = m_children.begin();
    it != m_children.end(); ++it )
  {
    FabricUI::ValueEditor::BaseModelItem *child = *it;
    delete child;
  }
}

FabricUI::ValueEditor::BaseModelItem *RootModelItem::getChild(
  FTL::CStrRef childName,
  bool doCreate
  )
{
  int numExisting = m_children.size();
  for (int i = 0; i < numExisting; i++)
  {
    if ( m_children[i]->getName() == childName )
      return m_children[i];
  }
  if (doCreate)
  {
    // Ensure this child exists, we can't assume its valid
    if ( getChildIndex( childName ) >= 0 )
    {
      FabricUI::ValueEditor::BaseModelItem* res = createChild( childName );
      m_children.push_back( res );
      return res;
    }
  }
  return NULL;
}

FabricUI::ValueEditor::BaseModelItem *RootModelItem::getChild( int index, bool doCreate )
{
  FTL::CStrRef childName = getChildName( index );
  return getChild( childName, doCreate );
}

int RootModelItem::getChildIndex( FTL::CStrRef name )
{
  int numChildren = getNumChildren();
  for (int i = 0; i < numChildren; i++)
  {
    if ( getChildName( i ) == name )
    {
      return i;
    }
  }
  return -1;
}

FabricUI::ValueEditor::BaseModelItem *RootModelItem::onPortRenamed(
  unsigned index,
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  for ( ChildVec::iterator it = m_children.begin();
    it != m_children.end(); ++it )
  {
    FabricUI::ValueEditor::BaseModelItem *childModelItem = *it;
    if ( childModelItem->getName() == oldName )
    {
      childModelItem->onRenamed(
        oldName,
        newName
        );
      return childModelItem;
    }
  }

  return 0;
}

bool RootModelItem::hasDefault()
{
  return false;
}

void RootModelItem::resetToDefault()
{
  assert( false );
}

//////////////////////////////////////////////////////////////////////////

std::string SplitLast( std::string& path )
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

std::string SplitFirst( std::string& path )
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

} // namespace ModelItems
} // namespace FabricUI
