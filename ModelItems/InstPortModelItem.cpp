//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/NodePortItemMetadata.h>
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

QVariant InstPortModelItem::GetValue()
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

bool InstPortModelItem::canRenameItem()
{
  FabricCore::DFGExec nodeExec = m_exec.getSubExec( m_nodeName.c_str() );
  return !nodeExec.editWouldSplitFromPreset();
}

void InstPortModelItem::RenameItem( const char* newName )
{
  std::string subExecPath = m_execPath;
  if ( !subExecPath.empty() )
    subExecPath += '.';
  subExecPath += m_nodeName;

  FabricCore::DFGExec subExec = m_exec.getSubExec( m_nodeName.c_str() );
  
  assert( !subExec.editWouldSplitFromPreset() );
  m_dfgUICmdHandler->dfgDoRenamePort(
    m_binding,
    subExecPath,
    subExec,
    m_portName,
    newName
    );
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
