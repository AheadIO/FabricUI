//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include "NodePortItemMetadata.h"

#include <FabricUI/ModelItems/SetPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

class SetPortItemMetadata : public NodePortItemMetadata
{
protected:

  SetPortModelItem *m_setPortModelItem;

public:

  SetPortItemMetadata( SetPortModelItem *setPortModelItem )
    : NodePortItemMetadata( setPortModelItem )
    , m_setPortModelItem( setPortModelItem )
    {}

  virtual const char* getString( const char* key ) const /*override*/
  {
    if ( key == VENotInspectableKey )
    {
      FabricCore::DFGExec exec = m_setPortModelItem->getExec();
      FTL::CStrRef portPath = m_setPortModelItem->getPortPath();
      bool isNotInspectable = exec.hasSrcPort( portPath.c_str() );
      return isNotInspectable? FTL_STR("1").c_str(): FTL_STR("").c_str();
    }

    return NodePortItemMetadata::getString( key );
  }
};

} // namespace ModelItems
} // namespace FabricUI
