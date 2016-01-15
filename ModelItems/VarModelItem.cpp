#include "VarModelItem.h"

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////

VarModelItem::VarModelItem( FabricCore::DFGExec& exec, const char* name )
  : NodeModelItem( exec, name )
  , m_name( name )
  , m_cname( name )
{
}


VarModelItem::~VarModelItem()
{
}

size_t VarModelItem::NumChildren()
{
  return 0;
}

BaseModelItem* VarModelItem::GetChild( int childIndex )
{
  return NULL;
}

QString VarModelItem::GetName()
{
  return m_name;
}

int VarModelItem::GetInOut()
{
  // Even though we are not actually a port,
  // we return IO because we can get & set our val
  return FabricCore::DFGPortType_IO;
}

QVariant VarModelItem::GetValue()
{
  return QVariant::fromValue( m_exec.getVarValue( m_cname.c_str() ) );
}

bool VarModelItem::hasDefault()
{
  return false;
}

void VarModelItem::resetToDefault()
{
}

void VarModelItem::onViewValueChanged( QVariant const& var, bool )
{
  FabricCore::RTVal val = m_exec.getVarValue( m_cname.c_str() );
  if (RTVariant::toRTVal( var, val ))
  {
    m_exec.setVarValue( m_cname.c_str(), val );
  }
}
