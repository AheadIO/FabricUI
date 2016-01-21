//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ModelItems/ArgModelItem.h>
#include <FabricUI/ModelItems/ModelItemMetadata.h>

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{

  namespace ModelItems
  {

    class ArgItemMetadata : public ModelItemMetadata
    {
    private:

      ArgModelItem *m_argModelItem;

    public:

      ArgItemMetadata( ArgModelItem *argModelItem )
        : m_argModelItem( argModelItem ) {}

      virtual const char* getString( const char* key ) const /*override*/
      {
        FabricCore::DFGExec rootExec = m_argModelItem->getRootExec();
        FTL::CStrRef argName = m_argModelItem->getArgName();
        return rootExec.getExecPortMetadata( argName.c_str(), key );
      }

    };

  }
}
