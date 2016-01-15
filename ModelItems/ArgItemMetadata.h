#pragma once

#include "PortItemMetadata.h"

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{

  namespace ModelItems
  {
    class ArgItemMetadata : public PortItemMetadata
    {
    public:
      ArgItemMetadata( FabricCore::DFGExec exec, const char* path )
        : PortItemMetadata( exec, path )
      {}

      virtual const char* getString( const char* key ) const /*override*/
      {
        return const_cast<FabricCore::DFGExec&>(m_exec).getExecPortMetadata( m_path.c_str(), key );
      }

    };
  }
}
