
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


QString NodeModelItem::GetName()
{
  return m_exec.getTitle();
}

size_t NodeModelItem::NumChildren()
{
  return m_exec.getNodePortCount( m_path.c_str() );
}

QString NodeModelItem::ChildName( int i )
{
  return m_exec.getNodePortName( m_path.c_str(), i );
}

BaseModelItem* NodeModelItem::CreateChild( QString name )
{
  QString childPath;
  if (!m_path.empty())
  {
    childPath.sprintf( "%s.", m_path.c_str() );
    childPath += name;
  }
  return new PortModelItem( m_exec, childPath );
}

ItemMetadata* NodeModelItem::GetMetadata()
{
  return NULL;
}

QVariant NodeModelItem::GetValue()
{
  return QString( m_exec.getInstTitle( m_path.c_str() ) );
}

void NodeModelItem::onViewValueChanged( QVariant const& var, bool commit )
{
  if (commit)
  {
    QByteArray asciiArr = var.toString().toAscii();
    m_exec.setInstTitle( m_path.c_str(), asciiArr.data() );
    emit modelValueChanged(var);
  }
}