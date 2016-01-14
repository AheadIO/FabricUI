
#include "NodeModelItem.h"
#include "PortModelItem.h"
#include <assert.h>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////

NodeModelItem::NodeModelItem( FabricCore::DFGExec exec, const char* path )
 : m_exec( exec )
 , m_path( path )
{

}

NodeModelItem::~NodeModelItem()
{

}

size_t NodeModelItem::NumChildren()
{
  return m_exec.getExecPortCount();
}

BaseModelItem* NodeModelItem::GetChild( QString childName )
{
  FabricCore::DFGExec exec = m_exec;

  int numExisting = m_children.size();
  for (int i = 0; i < numExisting; i++)
  {
    if (m_children[i]->GetName() == childName)
      return m_children[i];
  }
  //  we shouldn't ever create a child that doesn't exist!
  assert( ChildIndex( childName ) >= 0 );
  PortModelItem* res = new PortModelItem( m_exec, childName );
  m_children.push_back( res );
  return res;
}

BaseModelItem* NodeModelItem::GetChild( int index )
{
  QString childName = ChildName( index );
  return GetChild( childName );
}

int NodeModelItem::ChildIndex( QString name )
{
  int numChildren = m_exec.getExecPortCount();
  for (int i = 0; i < numChildren; i++)
  {
    if (m_exec.getExecPortName( i ) == name)
    {
      return i;
    }
  }
  return -1;
}

QString NodeModelItem::ChildName( int i )
{
  return m_exec.getExecPortName( i );
}

QString NodeModelItem::GetName()
{
  const char* title = m_exec.getTitle();
  if (title && *title != '\0')
    return title;
  return QString( "[Root Ports]" );
}

ItemMetadata* NodeModelItem::GetMetadata()
{
  return NULL;
}

QVariant NodeModelItem::GetValue()
{
  return QString( m_exec.getTitle() );
}

void NodeModelItem::argInserted( int index, const char* name, const char* type )
{
  m_children.insert( m_children.begin() + index, 
                     new PortModelItem( m_exec, name ) );
}

void NodeModelItem::argTypeChanged( int index, const char* name, const char* newType )
{
  BaseModelItem* pChild = GetChild( name );
  assert( pChild != NULL );
  if (pChild != NULL)
  {
    assert( pChild->GetName() == name );
    // TODO:  What?  Reset the QVariant...
  }
}

void NodeModelItem::argRemoved( int index, const char* name )
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

void NodeModelItem::onViewValueChanged( QVariant const& var, bool commit )
{
  if (commit)
  {
    QByteArray asciiArr = var.toString().toAscii();
    m_exec.setTitle( asciiArr.data() );
    //emit modelValueChanged(var);
  }
}

bool NodeModelItem::hasDefault()
{
  return false;
}

void NodeModelItem::resetToDefault()
{
}
