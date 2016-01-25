//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/NodePortItemMetadata.h>
#include <FabricUI/ModelItems/NodePortModelItem.h>
#include <FabricUI/ModelItems/RootModelItem.h>
#include <QtCore/QStringList>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
NodePortModelItem::NodePortModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName,
  FTL::StrRef portName
  )
  : m_dfgUICmdHandler( dfgUICmdHandler )
  , m_binding( binding )
  , m_execPath( execPath )
  , m_exec( exec )
  , m_nodeName( nodeName )
  , m_portName( portName )
  , m_metadata( 0 )
{
  updatePortPath();
}

NodePortModelItem::~NodePortModelItem()
{
  delete m_metadata;
}

void NodePortModelItem::updatePortPath()
{
  m_portPath = m_nodeName;
  m_portPath += '.';
  m_portPath += m_portName;
}

ItemMetadata* NodePortModelItem::getMetadata()
{
  if ( !m_metadata )
    m_metadata = new NodePortItemMetadata( this );

  return m_metadata;
}

void NodePortModelItem::setMetadataImp( const char* key, const char* value, bool canUndo ) /**/
{
  m_exec.setNodePortMetadata( m_portPath.c_str(), key, value, canUndo );
}

int NodePortModelItem::getInOut()
{
  return m_exec.getNodePortType( m_portPath.c_str() );
}

QVariant NodePortModelItem::getValue()
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
        m_exec.getInstPortResolvedDefaultValue( m_portPath.c_str(), ctype );
      assert( rtVal.isValid() );
      return QVariant::fromValue<FabricCore::RTVal>( rtVal.copy() );
    }
  }
  catch (FabricCore::Exception* e)
  {
    printf( "[ERROR] %s", e->getDesc_cstr() );
  }
  return QVariant();
}

void NodePortModelItem::setValue(
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
  FabricCore::DFGNotifBracket _( host );
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

void NodePortModelItem::onNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName
  )
{
  assert( m_nodeName == oldNodeName );

  m_nodeName = newNodeName;

  updatePortPath();
}

} // namespace ModelItems
} // namespace FabricUI
