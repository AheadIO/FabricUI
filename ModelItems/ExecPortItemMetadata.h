#pragma once

#include "PortItemMetadata.h"

namespace FabricUI
{
  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // MetaData specialization allows access to Port metadata
    class ExecPortItemMetadata : public PortItemMetadata
    {
    public:

      ExecPortItemMetadata( FabricCore::DFGExec exec, const char* path )
        : PortItemMetadata( exec, path )
      {}

      virtual const char* getString( const char* key ) const /*override*/
      {
        return const_cast<FabricCore::DFGExec&>(m_exec).getExecPortMetadata( m_path.c_str(), key );
      }
    };
  }
}
