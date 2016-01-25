//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/ArgItemMetadata.h>
#include <FabricUI/ModelItems/ArgModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <QtCore/QVariant>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
ArgModelItem::ArgModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef argName
  )
  : m_dfgUICmdHandler( dfgUICmdHandler )
  , m_binding( binding )
  , m_argName( argName )
  , m_rootExec( binding.getExec() )
  , m_metadata( 0 )
{
}

FTL::CStrRef ArgModelItem::getName()
{
  return m_argName;
}

bool ArgModelItem::canRename()
{
  return true;
}

void ArgModelItem::rename( FTL::CStrRef newName )
{
  m_dfgUICmdHandler->dfgDoRenamePort(
    m_binding,
    FTL::CStrRef(),
    m_rootExec,
    m_argName,
    newName
    );
}

void ArgModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  assert( m_argName == oldName );
  m_argName = newName;
}

QVariant ArgModelItem::getValue()
{
  FabricCore::RTVal rtVal = m_binding.getArgValue( m_argName.c_str() );
  // When an argument is first create it does not yet have a value
  // associated with it.
  if ( rtVal.isValid() )
    return QVariant::fromValue<FabricCore::RTVal>( rtVal.copy() );
  else
    return QVariant();
}


void ArgModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  // We get the value of the argument to ensure we
  // have an entity of the appropriate type.  This is because
  // we do not know what the type of the incoming value is
  // For example, we could create a port of type Float, but
  // the UI will build a Double slider, and send us a Double
  // In this case, the core will complain if we built a RTVal
  // from this type and tried to set it (because mis-matched type).
  FabricCore::RTVal curRTVal = m_binding.getArgValue( m_argName.c_str() );
  assert( curRTVal.isValid() );
  FabricCore::DFGHost host = m_binding.getHost();
  FabricCore::DFGNotifBracket _( host );
  FabricCore::Context context = host.getContext();
  FabricCore::RTVal rtVal =
    FabricCore::RTVal::Construct( context, curRTVal.getTypeNameCStr(), 0, 0 );
  assert( rtVal.isValid() );
  RTVariant::toRTVal( var, rtVal );
  if ( commit )
  {
    if ( valueAtInteractionBegin.isValid() )
    {
      RTVariant::toRTVal( valueAtInteractionBegin, rtVal );
      m_binding.setArgValue(
        m_argName.c_str(),
        rtVal,
        false // canUndo
        );
    }

    RTVariant::toRTVal( var, rtVal );
    m_dfgUICmdHandler->dfgDoSetArgValue(
      m_binding,
      m_argName,
      rtVal
      );
  }
  else
  {
    RTVariant::toRTVal( var, rtVal );
    m_binding.setArgValue(
      m_argName.c_str(),
      rtVal,
      false // canUndo
      );
  }
}

ItemMetadata* ArgModelItem::getMetadata()
{
  if ( !m_metadata )
    m_metadata = new ArgItemMetadata( this );
  return m_metadata; 
}

void ArgModelItem::setMetadataImp(
  const char* key, 
  const char* value,
  bool canUndo )
{
  m_rootExec.setExecPortMetadata( m_argName.c_str(), key, value, canUndo );
}

int ArgModelItem::getInOut()
{
  return m_rootExec.getExecPortType( m_argName.c_str() );
}

bool ArgModelItem::hasDefault()
{
  return false;
}

void ArgModelItem::resetToDefault()
{
  assert( false );
}

} // namespace ModelItems
} // namespace FabricUI
