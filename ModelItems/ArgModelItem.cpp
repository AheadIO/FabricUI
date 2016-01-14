#include "ArgModelItem.h"
#include "ArgItemMetadata.h"

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////
ArgModelItem::ArgModelItem( const FabricCore::DFGBinding & binding, QString portName )
  : PortModelItem(binding.getExec(), portName)
  , m_binding(binding)
{
}

QVariant ArgModelItem::GetValue()
{
  FabricCore::RTVal val = m_binding.getArgValue( m_cname.c_str() );
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
  FabricCore::RTVal val = m_binding.getArgValue( m_cname.c_str() );
  if (RTVariant::toRTVal( var, val ) )
  {
    m_binding.setArgValue( m_cname.c_str(), val, commit );
  }
}

ItemMetadata* FabricUI::ModelItems::ArgModelItem::GetMetadata()
{
  if (m_metadata == NULL)
    m_metadata = new ArgItemMetadata( m_exec, m_cname.c_str() );

  return m_metadata; 
}
