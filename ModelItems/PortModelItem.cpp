#include "PortModelItem.h"
#include "PortItemMetadata.h"

#include <QtCore/QStringList>
#include <assert.h>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////
PortModelItem::PortModelItem( const FabricCore::DFGExec& exec, QString portPath )
  : m_exec( exec )
//  , m_name( portName )
  , m_metadata( NULL )
{
  m_path = portPath.toStdString();
  // Strip off the path part and keep the name
  m_name = portPath.split( '.' ).last();
  
  // A debug port allows us to view the values
  // as they are being sent through the graph
  m_exec.addDebugNodePort( m_path.data() );
}

PortModelItem::~PortModelItem()
{
  m_exec.removeDebugNodePort( m_path.data() );
  if (m_metadata != NULL)
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
    m_metadata = new PortItemMetadata( m_exec, m_path.c_str() );
  return m_metadata;
}

int PortModelItem::GetInOut()
{
  return m_exec.getNodePortType( m_path.c_str() );
}

QVariant PortModelItem::GetValue()
{
  try
  {
    // TODO: Find a way to show values of connected ports
    //if (m_exec.hasSrcPort( m_cname.c_str() ))
    //  return QString( "<connected>" );

    // If we have a resolved type, allow getting the default val
    const char* ctype = m_exec.getNodePortResolvedType( m_path.c_str() );
    if (ctype != NULL)
    {
      FabricCore::RTVal val = 
        m_exec.getInstPortResolvedDefaultValue( m_path.c_str(), ctype );

      //FabricCore::DFGBinding binding = m_binding.bind();
      //  FabricCore::RTVal val = m_exec.getArgValue( m_cname.c_str() );
      if (val.isValid())
      {
        return QVariant::fromValue<FabricCore::RTVal>( val );
      }
    }
    {
      FabricCore::RTVal val = m_exec.getDebugNodePortValue( m_path.data() );
      return QVariant::fromValue<FabricCore::RTVal>( val );
    }
  }
  catch (FabricCore::Exception* e)
  {
    printf( "[ERROR] %s", e->getDesc_cstr() );
  }
  return QString( "|Invalid Port|" );
}

void PortModelItem::onViewValueChanged( QVariant const& vars, bool commit)
{
  if (m_exec.hasSrcPort( m_path.c_str() ))
    return;

  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getNodePortResolvedType( m_path.c_str() );
  if (ctype != NULL)
  {
    FabricCore::RTVal val = 
      m_exec.getInstPortResolvedDefaultValue( m_path.c_str(), ctype );

    if (RTVariant::toRTVal( vars, val ))
      m_exec.setPortDefaultValue( m_path.c_str(), val, commit );
  }
}

bool FabricUI::ModelItems::PortModelItem::hasDefault()
{
  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getNodePortResolvedType( m_path.c_str() );
  return (ctype != NULL);
}

void FabricUI::ModelItems::PortModelItem::resetToDefault()
{
#pragma message("Fix instance values for non-arg ports")
  //// If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getNodePortResolvedType( m_path.c_str() );
  if (ctype != NULL)
  {
    //FabricCore::RTVal val = m_exec.getInstPortResolvedDefaultValue(
    //  m_cname.c_str(),
    //  ctype );

    FabricCore::RTVal val = m_exec.getPortDefaultValue( m_path.c_str(), ctype );
    if (val.isValid())
      onViewValueChanged( QVariant::fromValue( val ), 1 );
  }
}
