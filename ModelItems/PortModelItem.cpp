#include "PortModelItem.h"
#include "PortItemMetadata.h"

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <QtCore/QStringList>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////
PortModelItem::PortModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName,
  FTL::StrRef portName,
  QString name
  )
  : m_dfgUICmdHandler( dfgUICmdHandler )
  , m_binding( binding )
  , m_execPath( execPath )
  , m_exec( exec )
  , m_name( name )
  , m_metadata( 0 )
{
  m_portPath = nodeName;
  m_portPath += '.';
  m_portPath += portName;
}

PortModelItem::~PortModelItem()
{
  delete m_metadata;
}

size_t PortModelItem::NumChildren()
{
  return 0;
}

BaseModelItem* PortModelItem::GetChild( int i )
{
  return NULL;
}

QString PortModelItem::GetName()
{
  return m_name;
}

ItemMetadata* PortModelItem::GetMetadata()
{
  if (m_metadata == NULL)
    m_metadata = new PortItemMetadata( m_exec, m_portPath.c_str() );
  return m_metadata;
}

int PortModelItem::GetInOut()
{
  return m_exec.getNodePortType( m_portPath.c_str() );
}

QVariant PortModelItem::GetValue()
{
  try
  {
    // TODO: Find a way to show values of connected ports
    //if (m_exec.hasSrcPort( m_cname.c_str() ))
    //  return QString( "<connected>" );

    // If we have a resolved type, allow getting the default val
    const char* ctype = m_exec.getNodePortResolvedType( m_portPath.c_str() );
    if (ctype != NULL)
    {
      FabricCore::RTVal rtVal = 
        m_exec.getInstPortResolvedDefaultValue( m_portPath.c_str(), ctype );
      assert( rtVal.isValid() );
      return QVariant::fromValue<FabricCore::RTVal>( rtVal.copy() );
    }
  }
  catch (FabricCore::Exception* e)
  {
    printf( "[ERROR] %s", e->getDesc_cstr() );
  }
  return QString( "|Invalid Port|" );
}

void PortModelItem::SetValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  if ( m_exec.hasSrcPort( m_portPath.c_str() ) )
    return;

  const char* resolvedTypeName =
    m_exec.getNodePortResolvedType( m_portPath.c_str() );
  assert( resolvedTypeName );
  if ( !resolvedTypeName )
    return;

  FabricCore::DFGHost host = m_binding.getHost();
  FabricCore::Context context = host.getContext();
  FabricCore::RTVal rtVal =
    FabricCore::RTVal::Construct( context, resolvedTypeName, 0, 0 );

  if ( commit )
  {
    if ( valueAtInteractionBegin.isValid() )
    {
      RTVariant::toRTVal( valueAtInteractionBegin, rtVal );
      m_exec.setPortDefaultValue(
        m_portPath.c_str(),
        rtVal
        );
    }

    RTVariant::toRTVal( value, rtVal );
    m_dfgUICmdHandler->dfgDoSetPortDefaultValue(
      m_binding,
      m_execPath,
      m_exec,
      m_portPath,
      rtVal
      );
  }
  else
  {
    RTVariant::toRTVal( value, rtVal );
    m_exec.setPortDefaultValue(
      m_portPath.c_str(),
      rtVal
      );
  }
}

bool FabricUI::ModelItems::PortModelItem::hasDefault()
{
  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getNodePortResolvedType( m_portPath.c_str() );
  return (ctype != NULL);
}

void FabricUI::ModelItems::PortModelItem::resetToDefault()
{
//#pragma message("Fix instance values for non-arg ports")
  //// If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getNodePortResolvedType( m_portPath.c_str() );
  if (ctype != NULL)
  {
    //FabricCore::RTVal val = m_exec.getInstPortResolvedDefaultValue(
    //  m_cname.c_str(),
    //  ctype );

    FabricCore::RTVal val = m_exec.getPortDefaultValue( m_portPath.c_str(), ctype );
    if (val.isValid())
      onViewValueChanged( QVariant::fromValue( val ) );
  }
}
