//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/NodeModelItem.h>
#include <FabricUI/ModelItems/NodePortModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////

NodeModelItem::NodeModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName
  )
 : m_dfgUICmdHandler( dfgUICmdHandler )
 , m_binding( binding )
 , m_execPath( execPath )
 , m_exec( exec )
 , m_nodeName( nodeName )
{
  assert( !m_nodeName.empty() );
}

NodeModelItem::~NodeModelItem()
{
}

bool NodeModelItem::matchesPath(
  FTL::StrRef execPath,
  FTL::StrRef nodeName 
  )
{
  return m_execPath == execPath && m_nodeName == nodeName;
}

FTL::CStrRef NodeModelItem::getName()
{
  return m_nodeName;
}

void NodeModelItem::renameItem( const char* newName )
{
  m_dfgUICmdHandler->dfgDoEditNode(
    m_binding,
    m_execPath,
    m_exec,
    m_nodeName,
    newName,
    FTL::StrRef(),
    FTL::StrRef()
    );
}

BaseModelItem *NodeModelItem::onNodePortRenamed(
  FTL::CStrRef execPath,
  FTL::CStrRef nodeName,
  FTL::CStrRef oldNodePortName,
  FTL::CStrRef newNodePortName
  )
{
  if ( m_execPath == execPath
    && m_nodeName == nodeName )
  {
    return RootModelItem::onNodePortRenamed(
      execPath,
      nodeName,
      oldNodePortName,
      newNodePortName
      );
  }
  else return 0;
}

BaseModelItem *NodeModelItem::onNodeRenamed(
  FTL::CStrRef execPath,
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName
  )
{
  if ( m_execPath == execPath
    && m_nodeName == oldNodeName )
  {
    m_nodeName = newNodeName;
    RootModelItem::onNodeRenamed(
      execPath,
      oldNodeName,
      newNodeName
      );
    return this;
  }
  else return 0;
}

int NodeModelItem::getNumChildren()
{
  return m_exec.getNodePortCount( m_nodeName.c_str() );
}

FTL::CStrRef NodeModelItem::getChildName( int i )
{
  return m_exec.getNodePortName( m_nodeName.c_str(), i );
}

ItemMetadata* NodeModelItem::getMetadata()
{
  return NULL;
}

void NodeModelItem::setMetadataImp( const char* key, const char* value, bool canUndo ) /**/
{
  // TODO: Do We need this?
}

} // namespace ModelItems
} // namespace FabricUI
