#include "ExecPortModelItem.h"
#include "ExecPortItemMetadata.h"

#include <assert.h>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////
ExecPortModelItem::ExecPortModelItem( const FabricCore::DFGExec& exec, QString name )
  : m_exec( exec )
  , m_name( name )
  , m_metadata( NULL )
{
  m_cname = name.toStdString();
  
  // A debug port allows us to view the values
  // as they are being sent through the graph
  m_exec.addDebugNodePort( m_cname.data() );
}

ExecPortModelItem::~ExecPortModelItem()
{
  m_exec.removeDebugNodePort( m_cname.data() );
  if (m_metadata != NULL)
    delete m_metadata;
}

size_t ExecPortModelItem::NumChildren()
{
  return 0;
}

BaseModelItem* ExecPortModelItem::GetChild( int i )
{
  return NULL;
}

QString ExecPortModelItem::GetName()
{
  return m_name;
}

ItemMetadata* ExecPortModelItem::GetMetadata()
{
  if (m_metadata == NULL)
    m_metadata = new ExecPortItemMetadata( m_exec, m_cname.c_str() );
  return m_metadata;
}

int ExecPortModelItem::GetInOut()
{
  return m_exec.getExecPortType( m_cname.c_str() );
}

QVariant ExecPortModelItem::GetValue()
{
  try
  {
    // If we have a resolved type, allow getting the default val
    const char* ctype = m_exec.getExecPortResolvedType( m_cname.c_str() );
    if (ctype != NULL)
    {
      FabricCore::RTVal val = 
        m_exec.getPortDefaultValue( m_cname.c_str(), ctype );

      if (val.isValid())
      {
        return QVariant::fromValue<FabricCore::RTVal>( val );
      }
    }
    {
      FabricCore::RTVal val = m_exec.getDebugNodePortValue( m_cname.data() );
      return QVariant::fromValue<FabricCore::RTVal>( val );
    }
  }
  catch (FabricCore::Exception* e)
  {
    printf( "[ERROR] %s", e->getDesc_cstr() );
  }
  return QString( "|Invalid Port|" );
}

void ExecPortModelItem::onViewValueChanged( QVariant const& vars, bool commit)
{
  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getExecPortResolvedType( m_cname.c_str() );
  if (ctype != NULL)
  {
    FabricCore::RTVal val = 
      m_exec.getPortDefaultValue( m_cname.c_str(), ctype );

    if (RTVariant::toRTVal( vars, val ))
      m_exec.setPortDefaultValue( m_cname.c_str(), val, commit );
  }
}

bool ExecPortModelItem::hasDefault()
{
  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getExecPortResolvedType( m_cname.c_str() );
  return (ctype != NULL);
}

void ExecPortModelItem::resetToDefault()
{
  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getExecPortResolvedType( m_cname.c_str() );
  if (ctype != NULL)
  {
    FabricCore::RTVal val = m_exec.getPortDefaultValue( m_cname.c_str(), ctype );
    if (val.isValid())
      onViewValueChanged( QVariant::fromValue( val ), 1 );
  }
}
