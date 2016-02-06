//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/VarPathModelItem.h>
#include <FabricUI/ModelItems/VarPathItemMetadata.h>
#include <QtCore/QStringList>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
VarPathModelItem::VarPathModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef refName
  )
  : m_dfgUICmdHandler( dfgUICmdHandler )
  , m_binding( binding )
  , m_execPath( execPath )
  , m_exec( exec )
  , m_refName( refName )
  , m_metadata( 0 )
{
}

VarPathModelItem::~VarPathModelItem()
{
  delete m_metadata;
}

FTL::CStrRef VarPathModelItem::getName()
{
  return FTL_STR("varPath");
}

bool VarPathModelItem::canRename()
{
  return false;
}

void VarPathModelItem::rename( FTL::CStrRef newName )
{
  assert( false );
}

void VarPathModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  assert( false );
}

QVariant VarPathModelItem::getValue()
{
  FTL::CStrRef varPath = m_exec.getRefVarPath( m_refName.c_str() );
  if ( !varPath.empty() )
    return QVariant( QString::fromUtf8( varPath.data(), varPath.size() ) );
  else
    return QVariant( QString() );
}

void VarPathModelItem::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  QString valueQString = value.toString();
  m_exec.setRefVarPath(
    m_refName.c_str(),
    valueQString.toUtf8().constData()
    );
}

bool VarPathModelItem::hasDefault()
{
  return false;
}

void VarPathModelItem::resetToDefault()
{
  assert( false );
}

ItemMetadata *VarPathModelItem::getMetadata()
{
  if ( !m_metadata )
    m_metadata = new VarPathItemMetadata( this );
  return m_metadata;
}

void VarPathModelItem::setMetadataImp(
  const char* key,
  const char* value,
  bool canUndo
  ) /**/
{
  assert( false );
}

} // namespace ModelItems
} // namespace FabricUI
