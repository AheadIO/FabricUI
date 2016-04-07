//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/InstModelItem.h>
#include <FabricUI/ModelItems/InstPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////

InstModelItem::InstModelItem(
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

InstModelItem::~InstModelItem()
{
}

FabricUI::ValueEditor::BaseModelItem *InstModelItem::createChild( FTL::CStrRef portName )
{
  return pushChild(new InstPortModelItem(
    m_dfgUICmdHandler,
    m_binding,
    m_execPath,
    m_exec,
    m_nodeName,
    portName
    ));
}

QVariant InstModelItem::getValue()
{
  return QVariant();
}

void InstModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  assert( false );
}

} // namespace ModelItems
} // namespace FabricUI
