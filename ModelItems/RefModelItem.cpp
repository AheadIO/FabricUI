//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/RefItemMetadata.h>
#include <FabricUI/ModelItems/RefModelItem.h>
#include <FabricUI/ModelItems/RefPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

RefModelItem::RefModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName
  )
  : NodeModelItem(
    dfgUICmdHandler,
    binding,
    execPath,
    exec,
    nodeName
    )
  , m_metadata( this )
{
}

RefModelItem::~RefModelItem()
{
}

int RefModelItem::getNumChildren()
{
  return 1 + NodeModelItem::getNumChildren();
}

FTL::CStrRef RefModelItem::getChildName( int i )
{
  if ( i == 0 )
    return FTL_STR("varPath");
  else
    return NodeModelItem::getChildName( i - 1 );
}

QVariant RefModelItem::getValue()
{
  QVariant result;
  try
  {
    FTL::CStrRef varPath = m_exec.getRefVarPath( getNodeName().c_str() );
    result = QVariant::fromValue( m_exec.getVarValue( varPath.c_str() ) );
  }
  catch ( ... ) {}
  return result;
}

void RefModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  try
  {
    FTL::CStrRef varPath = m_exec.getRefVarPath( getNodeName().c_str() );
    FabricCore::RTVal val = m_exec.getVarValue( varPath.c_str() );
    if ( FabricUI::ValueEditor::RTVariant::toRTVal( var, val ) )
      m_exec.setVarValue( varPath.c_str(), val );
  }
  catch ( ... ) {}
}

FabricUI::ValueEditor::ItemMetadata *RefModelItem::getMetadata()
{
  return &m_metadata;
}

} // namespace ModelItems
} // namespace FabricUI
