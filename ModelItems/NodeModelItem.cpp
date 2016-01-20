//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
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
  FTL::StrRef nodeName,
  QString name
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

void NodeModelItem::RenameItem( const char* newName )
{
  m_exec.renameNode( m_nodeName.c_str(), newName );
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

BaseModelItem *NodeModelItem::createChild( FTL::CStrRef name )
{
  return new NodePortModelItem(
    m_dfgUICmdHandler,
    m_binding,
    m_execPath,
    m_exec,
    m_nodeName,
    name
    );
}

ItemMetadata* NodeModelItem::GetMetadata()
{
  return NULL;
}

void NodeModelItem::SetMetadataImp( const char* key, const char* value, bool canUndo ) /**/
{
  // TODO: Do We need this?
}

QVariant NodeModelItem::GetValue()
{
  return QVariant(); // QString( m_exec.getInstTitle( m_nodeName.c_str() ) );
}

void NodeModelItem::SetValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  if (commit)
  {
    QByteArray asciiArr = var.toString().toAscii();
    m_exec.setInstTitle( m_nodeName.c_str(), asciiArr.data() );
    emitModelValueChanged(var);
  }
}

} // namespace ModelItems
} // namespace FabricUI
