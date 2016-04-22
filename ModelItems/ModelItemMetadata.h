//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_MODELITEMMETADATA_H
#define FABRICUI_MODELITEMS_MODELITEMMETADATA_H

#include <FabricUI/ValueEditor/ItemMetadata.h>

namespace FabricUI
{
  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // MetaData specialization allows access to Port metadata
    class ModelItemMetadata : public FabricUI::ValueEditor::ItemMetadata
    {
    public:

      virtual int getSInt32( const char* key ) const /*override*/
      {
        const char* data = getString( key );
        assert( data && data[0] );
        return atoi( data );
      }

      virtual double getFloat64( const char* key ) const /*override*/
      {
        const char* value = getString( key );
        assert( value && value[0] );
        return atof( value );
      }

      virtual const FTL::JSONObject* getDict( const char* key ) const /*override*/
      {
        //throw std::logic_error( "The method or operation is not implemented." );
        return NULL; // it seems metadata is not stored in JSON format after all
      }

      virtual const FTL::JSONArray* getArray( const char* key ) const /*override*/
      {
        //throw std::logic_error( "The method or operation is not implemented." );
        return NULL;
      }

      virtual bool has( const char* key ) const /*override*/
      {
        const char* val = getString( key );
        return val && val[0];
      }
    };
  }
}

#endif // FABRICUI_MODELITEMS_MODELITEMMETADATA_H
