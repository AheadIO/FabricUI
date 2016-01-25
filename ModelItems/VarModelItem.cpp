//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/VarModelItem.h>
#include <FabricUI/ModelItems/VarPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////

VarModelItem::VarModelItem(
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
{
}

VarModelItem::~VarModelItem()
{
}

BaseModelItem *VarModelItem::createChild( FTL::CStrRef portName )
{
  return new VarPortModelItem(
    m_dfgUICmdHandler,
    m_binding,
    m_execPath,
    m_exec,
    m_nodeName,
    portName
    );
}

int VarModelItem::getInOut()
{
  // Even though we are not actually a port,
  // we return IO because we can get & set our val
  return FabricCore::DFGPortType_IO;
}

QVariant VarModelItem::getValue()
{
  return QVariant::fromValue( m_exec.getVarValue( m_nodeName.c_str() ) );
}

void VarModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  FabricCore::RTVal val = m_exec.getVarValue( m_nodeName.c_str() );
  if (RTVariant::toRTVal( var, val ))
  {
    m_exec.setVarValue( m_nodeName.c_str(), val );
  }
}

} // namespace ModelItems
} // namespace FabricUI
