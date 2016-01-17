
#include "NodeModelItem.h"
#include "PortModelItem.h"
#include <assert.h>

using namespace FabricUI;
using namespace ModelItems;

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
 , m_name( name )
{
  assert( !m_nodeName.empty() );
}

NodeModelItem::~NodeModelItem()
{
}

bool FabricUI::ModelItems::NodeModelItem::matchesPath(
  FTL::StrRef execPath,
  FTL::StrRef nodeName 
  )
{
  return m_execPath == execPath && m_nodeName == nodeName;
}

QString NodeModelItem::GetName()
{
  return m_name;
}

void FabricUI::ModelItems::NodeModelItem::RenameItem( const char* newName )
{
  QByteArray oldName = m_name.toAscii();
  m_exec.renameNode( oldName.data(), newName );
}


void FabricUI::ModelItems::NodeModelItem::OnItemRenamed( QString newName )
{
  m_name = newName;
}

size_t NodeModelItem::NumChildren()
{
  return m_exec.getNodePortCount( m_nodeName.c_str() );
}

QString NodeModelItem::ChildName( int i )
{
  return m_exec.getNodePortName( m_nodeName.c_str(), i );
}

BaseModelItem* NodeModelItem::CreateChild( QString name )
{
  return new PortModelItem(
    m_dfgUICmdHandler,
    m_binding,
    m_execPath,
    m_exec,
    m_nodeName,
    name.toUtf8().constData(),
    name
    );
}

ItemMetadata* NodeModelItem::GetMetadata()
{
  return NULL;
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
