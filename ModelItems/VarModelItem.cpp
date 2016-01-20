//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/VarModelItem.h>

using namespace FabricUI;
using namespace ModelItems;

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

int VarModelItem::GetInOut()
{
  // Even though we are not actually a port,
  // we return IO because we can get & set our val
  return FabricCore::DFGPortType_IO;
}

QVariant VarModelItem::GetValue()
{
  return QVariant::fromValue( m_exec.getVarValue( m_nodeName.c_str() ) );
}

bool VarModelItem::hasDefault()
{
  return false;
}

void VarModelItem::resetToDefault()
{
}

void VarModelItem::SetValue(
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
