//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "DFGBindingNotifier.h"

#include <FTL/JSONValue.h>
#include <stdio.h>

namespace FabricUI {
namespace DFG {

DFGBindingNotifier::DFGBindingNotifier( FabricCore::DFGBinding binding )
  : m_binding( binding )
  , m_depth( 0 )
{
  m_binding.registerNotificationCallback(
    &BindingNotificationCallback,
    this
    );
}

DFGBindingNotifier::~DFGBindingNotifier()
{
  m_binding.unregisterNotificationCallback(
    &BindingNotificationCallback,
    this
    );
}

void DFGBindingNotifier::handle( FTL::CStrRef jsonStr )
{
  try
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );
    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
      );

    FTL::CStrRef descStr = jsonObject->getString( FTL_STR("desc") );
    if ( descStr == FTL_STR("dirty") )
    {
      emit dirty();
    }
    else if ( descStr == FTL_STR("argChanged") )
    {
      unsigned index = unsigned( jsonObject->getSInt32( FTL_STR("index") ) );
      FTL::CStrRef name = jsonObject->getString( FTL_STR("name") );

      emit argValueChanged( index, name );
    }
    else if ( descStr == FTL_STR("argInserted") )
    {
      unsigned index = unsigned( jsonObject->getSInt32( FTL_STR("index") ) );
      FTL::CStrRef name = jsonObject->getString( FTL_STR("name") );
      FTL::CStrRef type = jsonObject->getStringOrEmpty( FTL_STR( "type" ) );
      
      emit argInserted( index, name, type );
    }
    else if ( descStr == FTL_STR("argTypeChanged") )
    {
      unsigned index = unsigned( jsonObject->getSInt32( FTL_STR("index") ) );
      FTL::CStrRef name = jsonObject->getString( FTL_STR("name") );
      FTL::CStrRef type = jsonObject->getString( FTL_STR("newType") );
      
      emit argTypeChanged( index, name, type );
    }
    else if ( descStr == FTL_STR("argRemoved") )
    {
      unsigned index = unsigned( jsonObject->getSInt32( FTL_STR("index") ) );
      FTL::CStrRef name = jsonObject->getString( FTL_STR("name") );
      
      emit argRemoved( index, name );
    }
    else if ( descStr == FTL_STR("argsReordered") )
    {
      FTL::JSONArray const *newOrderJSONArray =
        jsonObject->getArray( FTL_STR("newOrder") );
      size_t newOrderSize = newOrderJSONArray->size();
      unsigned *newOrderData =
        (unsigned *)alloca( newOrderSize * sizeof( unsigned ) );
      for ( size_t i = 0; i < newOrderSize; ++i )
        newOrderData[i] = unsigned( newOrderJSONArray->getSInt32( i ) );

      emit argsReordered(
        FTL::ArrayRef<unsigned>( newOrderData, newOrderSize )
        );
    }
    else if ( descStr == FTL_STR("varInserted") )
    {
      FTL::CStrRef varName = jsonObject->getString( FTL_STR("varName") );
      FTL::CStrRef varPath = jsonObject->getString( FTL_STR("varPath") );
      FTL::CStrRef typeName = jsonObject->getString( FTL_STR("typeName") );
      FTL::CStrRef extDep = jsonObject->getString( FTL_STR("extDep") );

      emit varInserted(
        varName,
        varPath,
        typeName,
        extDep
        );
    }
    else if ( descStr == FTL_STR("varRemoved") )
    {
      FTL::CStrRef varName = jsonObject->getString( FTL_STR("varName") );
      FTL::CStrRef varPath = jsonObject->getString( FTL_STR("varPath") );

      emit varRemoved(
        varName,
        varPath
        );
    }
    else
    {
      fprintf(
        stderr,
        "DFGBindingNotifier::bindingNotificationCallback: warning: unhandled notification:\n%s\n",
        jsonStr.c_str()
        );
    }
  }
  catch ( FabricCore::Exception e )
  {
    printf(
      "DFGBindingNotifier::bindingNotificationCallback: caught Core exception: %s\n",
      e.getDesc_cstr()
      );
  }
  catch ( FTL::JSONException e )
  {
    printf(
      "DFGBindingNotifier::bindingNotificationCallback: caught FTL::JSONException: %s\n",
      e.getDescCStr()
      );
  }
}

void DFGBindingNotifier::bindingNotificationCallback( FTL::CStrRef jsonStr )
{
  if ( m_depth > 0 )
  {
    // Process later as we are already processing notifications
    m_queued.push_back( jsonStr );
    return;
  }

  DepthBracket _( this );

  handle( jsonStr );

  while ( !m_queued.empty() )
  {
    std::vector<std::string> queued;
    m_queued.swap( queued );
    for ( std::vector<std::string>::const_iterator it = queued.begin();
      it != queued.end(); ++it )
    {
      FTL::CStrRef queuedJSONStr = *it;
      handle( queuedJSONStr );
    }
  }
}

void DFGBindingNotifier::BindingNotificationCallback(
  void * userData,
  char const *jsonCString,
  uint32_t jsonLength
  )
{
  static_cast<DFGBindingNotifier *>(
    userData
    )->bindingNotificationCallback(
      FTL::CStrRef( jsonCString, jsonLength )
      );
}

} // namespace DFG
} // namespace FabricUI
