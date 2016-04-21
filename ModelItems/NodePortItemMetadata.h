//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_NODEPORTITEMMETADATA_H
#define FABRICUI_MODELITEMS_NODEPORTITEMMETADATA_H

#include "DFGModelItemMetadata.h"

#include <FabricUI/ModelItems/NodePortModelItem.h>

namespace FabricUI
{
  namespace ModelItems
  {

    //////////////////////////////////////////////////////////////////////////
    // MetaData specialization allows access to Port metadata
    class NodePortItemMetadata : public DFGModelItemMetadata
    {
    protected:

      NodePortModelItem *m_nodePortModelItem;

    public:

      NodePortItemMetadata( NodePortModelItem *nodePortModelItem )
        : m_nodePortModelItem( nodePortModelItem ) {}

      virtual const char* getString( const char* key ) const /*override*/
      {
        try
        {
          FabricCore::DFGExec exec = m_nodePortModelItem->getExec();
          FTL::CStrRef portPath = m_nodePortModelItem->getPortPath();

          if (strcmp( "uiReadOnly", key ) == 0)
          {
            // Override disabled for connected ports
            if ( exec.hasSrcPort( portPath.c_str() ) )
              return "1";
            if (exec.editWouldSplitFromPreset())
              return "1";
          }

          if ( key == VEPortTypeKey )
          {
            FabricCore::DFGExec exec = m_nodePortModelItem->getExec();
            FTL::CStrRef portPath = m_nodePortModelItem->getPortPath();
            return DFGPortTypeToVEPortType(
              exec.getNodePortType( portPath.c_str() )
              ).c_str();
          }

          if ( key == VENotInspectableKey )
          {
            FabricCore::DFGExec exec = m_nodePortModelItem->getExec();
            FTL::CStrRef portPath = m_nodePortModelItem->getPortPath();
            bool isNotInspectable =
              exec.getNodePortType( portPath.c_str() ) != FabricCore::DFGPortType_In
                || exec.hasSrcPort( portPath.c_str() );
            return isNotInspectable? FTL_STR("1").c_str(): FTL_STR("").c_str();
          }

          return exec.getNodePortMetadata( portPath.c_str(), key );
        }
        catch (FabricCore::Exception* e)
        {
          printf( "[ERROR] %s", e->getDesc_cstr() );
          return NULL;
        }
      }
    };
  }
}

#endif // FABRICUI_MODELITEMS_NODEPORTITEMMETADATA_H
