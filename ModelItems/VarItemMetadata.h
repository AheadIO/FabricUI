//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_VARITEMMETADATA_H
#define FABRICUI_MODELITEMS_VARITEMMETADATA_H

#include "DFGModelItemMetadata.h"

namespace FabricUI {
namespace ModelItems {

class VarModelItem;

class VarItemMetadata : public DFGModelItemMetadata
{
public:

  VarItemMetadata( VarModelItem *varModelItem ) {}

  virtual const char* getString( const char* key ) const /*override*/
  {
    if ( key == FTL_STR("vePortType") )
      return FTL_STR("IO").c_str();

    return 0;
  }
};

} // namespace ModelItems
} // namespace FabricUI

#endif // FABRICUI_MODELITEMS_VARITEMMETADATA_H
