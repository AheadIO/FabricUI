
#include "PresetModelItem.h"
#include "ExecPortModelItem.h"
#include <assert.h>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////

PresetModelItem::PresetModelItem( FabricCore::DFGExec exec )
 : m_exec( exec )
{

}

PresetModelItem::~PresetModelItem()
{

}


QString PresetModelItem::GetName()
{
  return m_exec.getTitle();
}

size_t PresetModelItem::NumChildren()
{
  return m_exec.getExecPortCount( );
}

QString PresetModelItem::ChildName( int i )
{
  return m_exec.getExecPortName( i );
}

BaseModelItem* PresetModelItem::CreateChild( QString name )
{
  return new ExecPortModelItem( m_exec, name );
}

ItemMetadata* PresetModelItem::GetMetadata()
{
  return NULL;
}

QVariant PresetModelItem::GetValue()
{
  return QString( m_exec.getTitle( ) );
}

void PresetModelItem::onViewValueChanged( QVariant const& var, bool commit )
{
  if (commit)
  {
    QByteArray asciiArr = var.toString().toAscii();
    m_exec.setTitle( asciiArr.data() );
    emit modelValueChanged(var);
  }
}
