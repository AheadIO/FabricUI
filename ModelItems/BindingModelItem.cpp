//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
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


int BindingModelItem::getNumChildren()
{
  return m_rootExec.getExecPortCount();
}

FTL::CStrRef BindingModelItem::getChildName( int i )
{
  return m_rootExec.getExecPortName( i );
}

FabricUI::ValueEditor::BaseModelItem *BindingModelItem::createChild( FTL::CStrRef name ) /**/
{
  return pushChild(new ArgModelItem(
    m_dfgUICmdHandler,
    m_binding,
    name
    ));
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

void BindingModelItem::setMetadataImp( 
  const char* key, 
  const char* value, 
  bool canUndo ) /**/
{
  m_rootExec.setMetadata( key, value, canUndo );
}

QVariant BindingModelItem::getValue()
{
  return QVariant();
}

FabricUI::ValueEditor::ItemMetadata* BindingModelItem::getMetadata()
{
  return NULL;
}

void BindingModelItem::setValue(
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

void BindingModelItem::argRemoved( unsigned index, FTL::CStrRef name )
{
  for ( ChildVec::iterator itr = m_children.begin();
    itr != m_children.end(); itr++ )
  {
    FabricUI::ValueEditor::BaseModelItem *childModelItem = *itr;
    if ( childModelItem->getName() == name )
    {
      delete childModelItem;
      m_children.erase( itr );
      break;
    }
  }
}

} // namespace ModelItems
} // namespace FabricUI
