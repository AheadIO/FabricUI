//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/InstPortItemMetadata.h>
#include <FabricUI/ModelItems/InstPortModelItem.h>
#include <FabricUI/ModelItems/RootModelItem.h>
#include <QtCore/QStringList>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
InstPortModelItem::InstPortModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName,
  FTL::StrRef portName
  )
  : NodePortModelItem(
    dfgUICmdHandler,
    binding,
    execPath,
    exec,
    nodeName,
    portName
    )
{
}

InstPortModelItem::~InstPortModelItem()
{
}

ItemMetadata *InstPortModelItem::getMetadata()
{
  if ( !m_metadata )
    m_metadata = new InstPortItemMetadata( this );

  return m_metadata;
}

QVariant InstPortModelItem::getValue()
{
  try
  {
    // TODO: Find a way to show values of connected ports
    if (m_exec.hasSrcPorts( m_portPath.c_str() ))
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

FTL::CStrRef InstPortModelItem::getName()
{
  return m_portName;
}

bool InstPortModelItem::canRename()
{
  FabricCore::DFGExec nodeExec = m_exec.getSubExec( m_nodeName.c_str() );
  return nodeExec.getExecPortIndex( m_portName.c_str() ) > 0
    && !nodeExec.editWouldSplitFromPreset();
}

void InstPortModelItem::rename( FTL::CStrRef newName )
{
  std::string subExecPath = m_execPath;
  if ( !subExecPath.empty() )
    subExecPath += '.';
  subExecPath += m_nodeName;

  FabricCore::DFGExec subExec = m_exec.getSubExec( m_nodeName.c_str() );
  
  assert( !subExec.editWouldSplitFromPreset() );
  m_dfgUICmdHandler->dfgDoRenamePort(
    m_binding,
    QString::fromUtf8( subExecPath.data(), subExecPath.size() ),
    subExec,
    QString::fromUtf8( m_portName.data(), m_portName.size() ),
    QString::fromUtf8( newName.data(), newName.size() )
    );
}

void InstPortModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  assert( m_portName == oldName );
  m_portName = newName;
  updatePortPath();
}

bool InstPortModelItem::hasDefault()
{
  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getNodePortResolvedType( m_portPath.c_str() );
  return (ctype != NULL);
}

void InstPortModelItem::resetToDefault()
{
//#pragma message("Fix instance values for non-arg ports")
  //// If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getNodePortResolvedType( m_portPath.c_str() );
  if (ctype != NULL)
  {
    FabricCore::DFGExec nodeExec = m_exec.getSubExec( m_nodeName.c_str() );

    FabricCore::RTVal val =
      nodeExec.getPortDefaultValue( m_portName.c_str(), ctype );
    if (val.isValid())
      onViewValueChanged( QVariant::fromValue( val ) );
  }
}

} // namespace ModelItems
} // namespace FabricUI
