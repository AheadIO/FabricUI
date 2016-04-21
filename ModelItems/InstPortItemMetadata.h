//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_INSTPORTITEMMETADATA_H
#define FABRICUI_MODELITEMS_INSTPORTITEMMETADATA_H

#include <FabricCore.h>
#include <FabricUI/ModelItems/NodePortItemMetadata.h>
#include <FabricUI/ModelItems/InstPortModelItem.h>

namespace FabricUI
{
  namespace ModelItems
  {

    //////////////////////////////////////////////////////////////////////////
    // MetaData specialization allows access to Port metadata
    class InstPortItemMetadata : public NodePortItemMetadata
    {
    public:

      InstPortItemMetadata( InstPortModelItem *instPortModelItem )
        : NodePortItemMetadata( instPortModelItem ) {}

      virtual const char* getString( const char* key ) const /*override*/
      {
        char const *res = NodePortItemMetadata::getString( key );
        if ( !res || !res[0] )
        {
          // If we don't have a result, we check the same port on the executable
          FabricCore::DFGExec exec = m_nodePortModelItem->getExec();
          FTL::CStrRef nodeName = m_nodePortModelItem->getNodeName();
          FTL::CStrRef portName = m_nodePortModelItem->getPortName();
          FabricCore::DFGExec subExec = exec.getSubExec( nodeName.c_str() );
          res = subExec.getExecPortMetadata( portName.c_str(), key );
        }
        return res;
      }
    };
  }
}

#endif // FABRICUI_MODELITEMS_INSTPORTITEMMETADATA_H
