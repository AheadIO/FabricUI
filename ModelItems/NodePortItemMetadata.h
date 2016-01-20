//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FabricUI/ModelItems/ModelItemMetadata.h>
#include <FabricUI/ModelItems/NodePortModelItem.h>

namespace FabricUI
{
  namespace ModelItems
  {

    //////////////////////////////////////////////////////////////////////////
    // MetaData specialization allows access to Port metadata
    class NodePortItemMetadata : public ModelItemMetadata
    {
    protected:

      NodePortModelItem *m_nodePortModelItem;

    public:

      NodePortItemMetadata( NodePortModelItem *nodePortModelItem )
        : m_nodePortModelItem( nodePortModelItem ) {}

      virtual const char* getString( const char* key ) const /*override*/
      {
        FabricCore::DFGExec exec = m_nodePortModelItem->getExec();
        FTL::CStrRef portPath = m_nodePortModelItem->getPortPath();

        if (strcmp( "disabled", key ) == 0)
        {
          // Override disabled for connected ports
          if ( exec.hasSrcPort( portPath.c_str() ) )
            return "1";
          if (exec.editWouldSplitFromPreset())
            return "1";
        }

        return exec.getNodePortMetadata( portPath.c_str(), key );
      }
    };
  }
}
