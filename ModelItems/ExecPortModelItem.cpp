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

QString ExecPortModelItem::GetName()
{
  return m_name;
}

void FabricUI::ModelItems::ExecPortModelItem::RenameItem( const char* name )
{
  m_exec.renameExecPort( m_cname.c_str(), name );
}


void FabricUI::ModelItems::ExecPortModelItem::OnItemRenamed( QString newName )
{
  m_name = newName;
  m_cname = newName.toStdString();
}

ItemMetadata* ExecPortModelItem::GetMetadata()
{
  if (m_metadata == NULL)
    m_metadata = new ExecPortItemMetadata( m_exec, m_cname.c_str() );
  return m_metadata;
}

void FabricUI::ModelItems::ExecPortModelItem::SetMetadataImp( const char* key, const char* value, bool canUndo ) /**/
{
  m_exec.setExecPortMetadata( m_cname.c_str(), key, value, canUndo );
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
  return QVariant();
}

void ExecPortModelItem::SetValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getExecPortResolvedType( m_cname.c_str() );
  if (ctype != NULL)
  {
    FabricCore::RTVal rtVal = 
      m_exec.getPortDefaultValue( m_cname.c_str(), ctype );

    if (RTVariant::toRTVal( value, rtVal ))
      m_exec.setPortDefaultValue( m_cname.c_str(), rtVal, commit );
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
      onViewValueChanged( QVariant::fromValue( val ) );
  }
}
