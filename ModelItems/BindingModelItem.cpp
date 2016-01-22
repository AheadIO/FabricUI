//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/ModelItems/ArgModelItem.h>
#include <FabricUI/ModelItems/BindingModelItem.h>

namespace FabricUI {
namespace ModelItems {

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


bool BindingModelItem::matchesPath(
  FTL::StrRef execPath,
  FTL::StrRef nodeName
  )
{
  return execPath.empty() && nodeName.empty();
}

int BindingModelItem::getNumChildren()
{
  return m_rootExec.getExecPortCount();
}

FTL::CStrRef BindingModelItem::getChildName( int i )
{
  return m_rootExec.getExecPortName( i );
}

BaseModelItem *BindingModelItem::createChild( FTL::CStrRef name ) /**/
{
  return new ArgModelItem(
    m_dfgUICmdHandler,
    m_binding,
    name
    );
}

FTL::CStrRef BindingModelItem::getName()
{
  return FTL_STR("<Root>");
}

bool BindingModelItem::canRename()
{
  return false;
}

void BindingModelItem::rename( FTL::CStrRef newName )
{
  assert( false );
}

void BindingModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  assert( false );
}

void BindingModelItem::SetMetadataImp( 
  const char* key, 
  const char* value, 
  bool canUndo ) /**/
{
  m_rootExec.setMetadata( key, value, canUndo );
}

QVariant BindingModelItem::GetValue()
{
  return QVariant();// GetName();
}

ItemMetadata* BindingModelItem::GetMetadata()
{
  return NULL;
}

void BindingModelItem::SetValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  if (commit)
  {
    QByteArray asciiArr = var.toString().toAscii();
    m_rootExec.setTitle( asciiArr.data() );
    emitModelValueChanged(var);
  }
}

} // namespace ModelItems
} // namespace FabricUI
