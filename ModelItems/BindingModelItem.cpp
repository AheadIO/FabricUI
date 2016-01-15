#include "BindingModelItem.h"
#include "ArgModelItem.h"
#include <assert.h>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////
BindingModelItem::BindingModelItem( FabricCore::DFGBinding binding )
  : m_binding(binding)
{
  assert( m_binding.isValid() );
}

BindingModelItem::~BindingModelItem()
{

}

size_t BindingModelItem::NumChildren()
{
  return m_binding.getExec().getExecPortCount();
}

QString BindingModelItem::ChildName( int i )
{
  return m_binding.getExec().getExecPortName( i );
}

BaseModelItem* BindingModelItem::CreateChild( QString name ) /**/
{
  return new ArgModelItem( m_binding, name );
}

QString BindingModelItem::GetName()
{
  const char* title = m_binding.getExec().getTitle();
  if (title && *title != '\0')
    return title;
  return QString();
}

QVariant BindingModelItem::GetValue()
{
  return GetName();
}

ItemMetadata* BindingModelItem::GetMetadata()
{
  return NULL;
}

void BindingModelItem::onViewValueChanged( QVariant const& var, bool commit )
{
  if (commit)
  {
    QByteArray asciiArr = var.toString().toAscii();
    m_binding.getExec().setTitle( asciiArr.data() );
    emit modelValueChanged(var);
  }
}
