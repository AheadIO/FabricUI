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

bool PortModelItem::canRenameItem()
{
  size_t split = m_portPath.rfind( '.' );
  std::string nodeName = m_portPath.substr( 0, split );
  FabricCore::DFGExec nodeExec = m_exec.getSubExec( nodeName.c_str() );
  return !nodeExec.editWouldSplitFromPreset();
}

void FabricUI::ModelItems::PortModelItem::RenameItem( const char* newName )
{
  size_t split = m_portPath.rfind( '.' );
  std::string nodeName = m_portPath.substr( 0, split );
  std::string portName = m_portPath.substr( split + 1 );
  FabricCore::DFGExec nodeExec = m_exec.getSubExec( nodeName.c_str() );
  if (!nodeExec.editWouldSplitFromPreset())
  {
    nodeExec.renameExecPort( portName.c_str(), newName );
  }
}


void FabricUI::ModelItems::PortModelItem::OnItemRenamed( QString newName )
{
  size_t split = m_portPath.rfind( '.' );
  std::string nodeName = m_portPath.substr( 0, split );

  m_portPath = nodeName.c_str();
  m_portPath += '.';
  m_portPath += newName.toStdString();
}

ItemMetadata* PortModelItem::GetMetadata()
{
  if (m_metadata == NULL)
    m_metadata = new PortItemMetadata( m_exec, m_portPath.c_str() );
  return m_metadata;
}

void PortModelItem::SetMetadata( const char* key, const char* value, bool canUndo ) /**/
{
  m_exec.setNodePortMetadata( m_portPath.c_str(), key, value, canUndo );
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
        rtVal,
        false // canUndo
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
      rtVal,
      false // canUndo
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
    size_t split = m_portPath.rfind( '.' );
    std::string nodeName = m_portPath.substr( 0, split );
    std::string portName= m_portPath.substr( split + 1);
    FabricCore::DFGExec nodeExec = m_exec.getSubExec( nodeName.c_str() );
    //FabricCore::RTVal val = m_exec.getInstPortResolvedDefaultValue(
    //  m_cname.c_str(),
    //  ctype );

    FabricCore::RTVal val = nodeExec.getPortDefaultValue( portName.c_str(), ctype );
    if (val.isValid())
      onViewValueChanged( QVariant::fromValue( val ) );
  }
}
