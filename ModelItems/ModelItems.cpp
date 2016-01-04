
#include "ModelItems.h"
#include "assert.h"

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////

ExecModelItem::ExecModelItem( FabricCore::DFGExec& exec ) : m_exec( exec )
{

}

ExecModelItem::~ExecModelItem()
{

}

size_t ExecModelItem::NumChildren()
{
  return m_exec.getExecPortCount();
}

BaseModelItem* ExecModelItem::GetChild( QString childName )
{
  FabricCore::DFGExec exec = m_exec;

  int numExisting = m_children.size();
  for (int i = 0; i < numExisting; i++)
  {
    if (m_children[i]->GetName() == childName)
      return m_children[i];
  }
  //  we shouldn't ever create a child that doesn't exist!
  assert( ChildIndex( childName ) >= 0 );
  PortModelItem* res = new PortModelItem( m_exec, childName );
  m_children.push_back( res );
  return res;
}

BaseModelItem* ExecModelItem::GetChild( int index )
{
  QString childName = ChildName( index );
  return GetChild( childName );
}

int ExecModelItem::ChildIndex( QString name )
{
  int numChildren = m_exec.getExecPortCount();
  for (int i = 0; i < numChildren; i++)
  {
    if (m_exec.getExecPortName( i ) == name)
    {
      return i;
    }
  }
  return -1;
}

QString ExecModelItem::ChildName( int i )
{
  return m_exec.getExecPortName( i );
}

QString ExecModelItem::GetName()
{
  const char* title = m_exec.getTitle();
  if (title && *title != '\0')
    return title;
  return QString( "[Root Ports]" );
}

FTL::JSONObject* ExecModelItem::GetMetadata()
{
  return NULL;
}

QVariant ExecModelItem::GetValue()
{
  return QString( m_exec.getTitle() );
}

void ExecModelItem::argInserted( int index, const char* name, const char* type )
{
  m_children.insert( m_children.begin() + index, 
                     new PortModelItem( m_exec, name ) );
}

void ExecModelItem::argTypeChanged( int index, const char* name, const char* newType )
{
  BaseModelItem* pChild = GetChild( name );
  assert( pChild != NULL );
  if (pChild != NULL)
  {
    assert( pChild->GetName() == name );
    // TODO:  What?  Reset the QVariant...
  }
}

void ExecModelItem::argRemoved( int index, const char* name )
{
  for (ChildVec::iterator itr = m_children.begin(); itr != m_children.end(); itr++)
  {
    if ((*itr)->GetName() == name)
    {
      delete *itr;
      m_children.erase( itr );
      break;
    }
  }
}

void ExecModelItem::onViewValueChanged( QVariant const& var, bool commit )
{
  if (commit)
  {
    QByteArray asciiArr = var.toString().toAscii();
    m_exec.setTitle( asciiArr.data() );
    //emit modelValueChanged(var);
  }
}

//////////////////////////////////////////////////////////////////////////
BindingModelItem::BindingModelItem( FabricCore::DFGBinding binding )
  : ExecModelItem(binding.getExec())
  , m_binding(binding)
{
  assert( m_binding.isValid() );
}

BaseModelItem* BindingModelItem::GetChild( QString childName )
{
  int numExisting = m_children.size();
  for (int i = 0; i < numExisting; i++)
  {
    if (m_children[i]->GetName() == childName)
      return m_children[i];
  }
  //  we shouldn't ever create a child that doesn't exist!
  assert( ChildIndex( childName ) >= 0 );
  BaseModelItem* res = new ArgModelItem( m_binding, childName );
  m_children.push_back( res );
  return res;
}

void BindingModelItem::argInserted( int index, const char * name, const char * type )
{
  m_children.insert( m_children.begin() + index,
                     new ArgModelItem( m_binding, name ) );
}

//////////////////////////////////////////////////////////////////////////
PortModelItem::PortModelItem( const FabricCore::DFGExec& exec, QString portName )
  : m_exec( exec )
  , m_name( portName )
{
  m_cname = m_name.toStdString();
  assert( exec.haveExecPort( m_cname.c_str() ) );

  // A debug port allows us to view the values
  // as they are being sent through the graph
  m_exec.addDebugNodePort( m_cname.data() );
}

PortModelItem::~PortModelItem()
{
  m_exec.removeDebugNodePort( m_cname.data() );
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

FTL::JSONObject* PortModelItem::GetMetadata()
{
  //m_binding.getMetadata();
  return NULL;
  //m_binding.get_
  //m_exec.getPortMetadata(m_name);
}

QVariant PortModelItem::GetValue()
{
  try
  {
    // TODO: Find a way to show values of connected ports
    //if (m_exec.hasSrcPort( m_cname.c_str() ))
    //  return QString( "<connected>" );

    // If we have a resolved type, allow getting the default val
    const char* ctype = m_exec.getExecPortResolvedType( m_cname.c_str() );
    if (ctype != NULL)
    {
      FabricCore::RTVal val = m_exec.getPortDefaultValue( m_cname.c_str(), ctype );
      //FabricCore::DFGBinding binding = m_binding.bind();
      //  FabricCore::RTVal val = m_exec.getArgValue( m_cname.c_str() );
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

void PortModelItem::onViewValueChanged( QVariant const& vars, bool commit)
{
  if (m_exec.hasSrcPort( m_cname.c_str() ))
    return;

  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getExecPortResolvedType( m_cname.c_str() );
  if (ctype != NULL)
  {
    FabricCore::RTVal val = m_exec.getPortDefaultValue( m_cname.c_str(), ctype );
    if (RTVariant::toRTVal( vars, val ))
      m_exec.setPortDefaultValue( m_cname.c_str(), val, commit );
  }
}

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
  return QString( "|Invalid Port|" );
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
