//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "DFGBindingNotifier.h"

#include <FTL/JSONValue.h>
#include <stdio.h>

namespace FabricUI {
namespace DFG {

void DFGBindingNotifier::handle( FTL::CStrRef jsonStr )
{
  try
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );
    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
      );

    // fprintf(
    //   stderr,
    //   "DFGBindingNotifier::bindingNotificationCallback: received:\n%s\n",
    //   jsonStr.c_str()
    //   );

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
    else if ( descStr == FTL_STR("argRenamed") )
    {
      unsigned index = unsigned( jsonObject->getSInt32( FTL_STR("index") ) );
      FTL::CStrRef oldName = jsonObject->getString( FTL_STR("oldName") );
      FTL::CStrRef newName = jsonObject->getString( FTL_STR("newName") );
      
      emit argRenamed( index, oldName, newName );
    }
    else if ( descStr == FTL_STR("argRemoved") )
    {
      unsigned index = unsigned( jsonObject->getSInt32( FTL_STR("index") ) );
      FTL::CStrRef name = jsonObject->getString( FTL_STR("name") );
      
      emit argRemoved( index, name );
    }
    else if ( descStr == FTL_STR("argTypeChanged") )
    {
      unsigned index = unsigned( jsonObject->getSInt32( FTL_STR("index") ) );
      FTL::CStrRef name = jsonObject->getString( FTL_STR("name") );
      FTL::CStrRef type = jsonObject->getString( FTL_STR("newType") );
      
      emit argTypeChanged( index, name, type );
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

} // namespace DFG
} // namespace FabricUI
