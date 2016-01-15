#include "ArgModelItem.h"
#include "ArgItemMetadata.h"

#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <QtCore/QVariant>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////
ArgModelItem::ArgModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef argName
  )
  : /*m_dfgUICmdHandler( dfgUICmdHandler )
  , */m_binding( binding )
  , m_argName( argName )
  , m_rootExec( binding.getExec() )
  , m_metadata( 0 )
{
}

size_t ArgModelItem::NumChildren()
{
  return 0;
}

BaseModelItem* ArgModelItem::GetChild( int i )
{
  return NULL;
}

QString ArgModelItem::GetName()
{
  return QString::fromUtf8( m_argName.data(), m_argName.size() );
}

QVariant ArgModelItem::GetValue()
{
  FabricCore::RTVal val = m_binding.getArgValue( m_argName.c_str() );
  if (val.isValid())
  {
    return QVariant::fromValue<FabricCore::RTVal>( val );
  }
  return QVariant(); // QString( "|Invalid Port|" );
}

void ArgModelItem::onViewValueChanged( QVariant const& var, bool commit )
{
  // We get the value of the argument to ensure we
  // have an entity of the appropriate type.  This is because
  // we do not know what the type of the incoming value is
  // For example, we could create a port of type Float, but
  // the UI will build a Double slider, and send us a Double
  // In this case, the core will complain if we built a RTVal
  // from this type and tried to set it (because mis-matched type).
  FabricCore::RTVal val = m_binding.getArgValue( m_argName.c_str() );
  if (RTVariant::toRTVal( var, val ) )
  {
    m_binding.setArgValue( m_argName.c_str(), val, commit );
  }
}

ItemMetadata* FabricUI::ModelItems::ArgModelItem::GetMetadata()
{
  if (m_metadata == NULL)
    m_metadata = new ArgItemMetadata( m_rootExec, m_argName.c_str() );

  return m_metadata; 
}

int FabricUI::ModelItems::ArgModelItem::GetInOut()
{
  return m_rootExec.getExecPortType( m_argName.c_str() );
}

bool FabricUI::ModelItems::ArgModelItem::hasDefault()
{
  return false;
}

void FabricUI::ModelItems::ArgModelItem::resetToDefault()
{
  assert( false );
}
