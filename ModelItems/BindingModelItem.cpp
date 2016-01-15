#include "BindingModelItem.h"
#include "ArgModelItem.h"
#include <assert.h>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////
BindingModelItem::BindingModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding
  )
  : m_dfgUICmdHandler( dfgUICmdHandler )
  , m_binding( binding )
  , m_rootExec( binding.getExec() )
{
  assert( m_binding.isValid() );
  assert( m_rootExec.isValid() );
}

BindingModelItem::~BindingModelItem()
{
}


bool FabricUI::ModelItems::BindingModelItem::matchesPath(
  FTL::StrRef execPath,
  FTL::StrRef nodeName
  )
{
  return execPath.empty() && nodeName.empty();
}

size_t BindingModelItem::NumChildren()
{
  return m_rootExec.getExecPortCount();
}

QString BindingModelItem::ChildName( int i )
{
  return m_rootExec.getExecPortName( i );
}

BaseModelItem* BindingModelItem::CreateChild( QString name ) /**/
{
  return new ArgModelItem(
    m_dfgUICmdHandler,
    m_binding,
    name.toUtf8().constData()
    );
}

QString BindingModelItem::GetName()
{
  const char* title = m_rootExec.getTitle();
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

void BindingModelItem::onViewValueChangedImpl( QVariant const& var, bool commit )
{
  if (commit)
  {
    QByteArray asciiArr = var.toString().toAscii();
    m_rootExec.setTitle( asciiArr.data() );
    emitModelValueChanged(var);
  }
}
