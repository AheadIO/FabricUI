#pragma once

#include <FabricUI/ValueEditor/ItemMetadata.h>
#include <FabricCore.h>
#include <stdexcept>

class PortItemMetadata;

namespace FabricUI
{
  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // MetaData specialization allows access to Port metadata
    class PortItemMetadata : public ItemMetadata
    {
    protected:
      FabricCore::DFGExec m_exec;
      std::string m_path;

    public:

      PortItemMetadata( FabricCore::DFGExec exec, const char* path )
        : m_exec( exec )
        , m_path( path )
      {}

      virtual const char* getString( const char* key ) const /*override*/
      {
        if (strcmp( "disabled", key ) == 0)
        {
          // Override disabled for connected ports
          if ( m_exec.hasSrcPort( m_path.c_str() ) )
            return "1";
        }

        return const_cast<FabricCore::DFGExec&>(m_exec).getNodePortMetadata( m_path.c_str(), key );
      }

      virtual int getSInt32( const char* key ) const /*override*/
      {
        const char* data = getString( key );
        return atoi( data );
      }

      virtual double getFloat64( const char* key ) const /*override*/
      {
        const char* value = getString( key );
        return atof( value );
      }

      virtual const FTL::JSONObject* getDict( const char* key ) const /*override*/
      {
        throw std::logic_error( "The method or operation is not implemented." );
      }

      virtual const FTL::JSONArray* getArray( const char* key ) const /*override*/
      {
        throw std::logic_error( "The method or operation is not implemented." );
      }

      virtual bool has( const char* key ) const /*override*/
      {
        const char* val = getString( key );
        return strcmp( val, "" ) != 0;
      }
    };
  }
}
