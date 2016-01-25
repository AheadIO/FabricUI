//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/NodePortItemMetadata.h>
#include <FabricUI/ModelItems/VarPortModelItem.h>
#include <FabricUI/ModelItems/RootModelItem.h>
#include <QtCore/QStringList>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
VarPortModelItem::VarPortModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName,
  FTL::StrRef portName
  )
  : NodePortModelItem(
    dfgUICmdHandler,
    binding,
    execPath,
    exec,
    nodeName,
    portName
    )
{
}

VarPortModelItem::~VarPortModelItem()
{
}

FTL::CStrRef VarPortModelItem::getName()
{
  return FTL_STR("initialValue");
}

bool VarPortModelItem::canRename()
{
  return false;
}

void VarPortModelItem::rename( FTL::CStrRef newName )
{
  assert( false );
}

void VarPortModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  assert( false );
}

QVariant VarPortModelItem::getValue()
{
  try
  {
    // TODO: Find a way to show values of connected ports
    if (m_exec.hasSrcPort( m_portPath.c_str() ))
      return QVariant();

    // If we have a resolved type, allow getting the default val
    const char* ctype = m_exec.getNodePortResolvedType( m_portPath.c_str() );
    if (ctype != NULL)
    {
      FabricCore::RTVal rtVal = 
        m_exec.getPortDefaultValue( m_portPath.c_str(), ctype );
      if ( rtVal.isValid() )
      {
        // DFG returns references
        rtVal = rtVal.copy();
      }
      else
      {
        FabricCore::DFGHost host = m_exec.getHost();
        FabricCore::Context context = host.getContext();
        rtVal = FabricCore::RTVal::Construct( context, ctype, 0, 0 );
      }
      return QVariant::fromValue<FabricCore::RTVal>( rtVal.copy() );
    }
  }
  catch (FabricCore::Exception* e)
  {
    printf( "[ERROR] %s", e->getDesc_cstr() );
  }
  return QVariant();
}

bool VarPortModelItem::hasDefault()
{
  return false;
}

void VarPortModelItem::resetToDefault()
{
  assert( false );
}

} // namespace ModelItems
} // namespace FabricUI
