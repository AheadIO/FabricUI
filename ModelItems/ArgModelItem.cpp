#include "ArgModelItem.h"
#include "ArgItemMetadata.h"

#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <QtCore/QVariant>

using namespace FabricUI;
using namespace ModelItems;

//////////////////////////////////////////////////////////////////////////
ArgModelItem::ArgModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef argName
  )
  : m_dfgUICmdHandler( dfgUICmdHandler )
  , m_binding( binding )
  , m_argName( argName )
  , m_rootExec( binding.getExec() )
  , m_metadata( 0 )
{
}

size_t ArgModelItem::NumChildren()
{
  return 0;
}

BaseModelItem* ArgModelItem::GetChild( int i )
{
  return NULL;
}

QString ArgModelItem::GetName()
{
  return QString::fromUtf8( m_argName.data(), m_argName.size() );
}

void ArgModelItem::RenameItem( const char* name )
{
  m_rootExec.renameExecPort( m_argName.c_str(), name );
}

void FabricUI::ModelItems::ArgModelItem::OnItemRenamed( QString newName ) /**/
{
  m_argName = newName.toStdString();
}

QVariant ArgModelItem::GetValue()
{
  FabricCore::RTVal rtVal = m_binding.getArgValue( m_argName.c_str() );
  // When an argument is first create it does not yet have a value
  // associated with it.
  if ( rtVal.isValid() )
    return QVariant::fromValue<FabricCore::RTVal>( rtVal.copy() );
  else
    return QVariant();
}

void ArgModelItem::SetValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  // We get the value of the argument to ensure we
  // have an entity of the appropriate type.  This is because
  // we do not know what the type of the incoming value is
  // For example, we could create a port of type Float, but
  // the UI will build a Double slider, and send us a Double
  // In this case, the core will complain if we built a RTVal
  // from this type and tried to set it (because mis-matched type).
  FabricCore::RTVal curRTVal = m_binding.getArgValue( m_argName.c_str() );
  assert( curRTVal.isValid() );
  FabricCore::DFGHost host = m_binding.getHost();
  FabricCore::Context context = host.getContext();
  FabricCore::RTVal rtVal =
    FabricCore::RTVal::Construct( context, curRTVal.getTypeNameCStr(), 0, 0 );
  assert( rtVal.isValid() );
  RTVariant::toRTVal( var, rtVal );
  if ( commit )
  {
    if ( valueAtInteractionBegin.isValid() )
    {
      RTVariant::toRTVal( valueAtInteractionBegin, rtVal );
      m_binding.setArgValue(
        m_argName.c_str(),
        rtVal,
        false // canUndo
        );
    }

    RTVariant::toRTVal( var, rtVal );
    m_dfgUICmdHandler->dfgDoSetArgValue(
      m_binding,
      m_argName,
      rtVal
      );
  }
  else
  {
    RTVariant::toRTVal( var, rtVal );
    m_binding.setArgValue(
      m_argName.c_str(),
      rtVal,
      false // canUndo
      );
  }
}

ItemMetadata* FabricUI::ModelItems::ArgModelItem::GetMetadata()
{
  if (m_metadata == NULL)
    m_metadata = new ArgItemMetadata( m_rootExec, m_argName.c_str() );

  return m_metadata; 
}

void FabricUI::ModelItems::ArgModelItem::SetMetadata(
  const char* key, 
  const char* value,
  bool canUndo )
{
  m_rootExec.setExecPortMetadata( m_argName.c_str(), key, value, canUndo );
}

int FabricUI::ModelItems::ArgModelItem::GetInOut()
{
  return m_rootExec.getExecPortType( m_argName.c_str() );
}

bool FabricUI::ModelItems::ArgModelItem::hasDefault()
{
  return false;
}

void FabricUI::ModelItems::ArgModelItem::resetToDefault()
{
  assert( false );
}
