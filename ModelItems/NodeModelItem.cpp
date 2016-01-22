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

bool NodeModelItem::canRename()
{
  return !m_exec.editWouldSplitFromPreset();
}

void NodeModelItem::rename( FTL::CStrRef newName )
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

void NodeModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  assert( m_nodeName == oldName );

  m_nodeName = newName;

  for ( ChildVec::iterator it = m_children.begin();
    it != m_children.end(); ++it )
  {
    NodePortModelItem *nodePortModelItem =
      static_cast<NodePortModelItem *>( *it );
    nodePortModelItem->onNodeRenamed( oldName, newName );
  }
}

int NodeModelItem::getNumChildren()
{
  return m_exec.getNodePortCount( m_nodeName.c_str() );
}

FTL::CStrRef NodeModelItem::getChildName( int i )
{
  return m_exec.getNodePortName( m_nodeName.c_str(), i );
}

ItemMetadata* NodeModelItem::GetMetadata()
{
  return NULL;
}

void NodeModelItem::SetMetadataImp( const char* key, const char* value, bool canUndo ) /**/
{
  // TODO: Do We need this?
}

} // namespace ModelItems
} // namespace FabricUI
