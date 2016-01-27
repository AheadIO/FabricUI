//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGUIUtil__
#define __UI_DFG_DFGUIUtil__

#include <FabricCore.h>
#include <FabricUI/DFG/DFGUINamespace.h>
#include <FTL/CStrRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

FabricCore::RTVal DFGCreateDefaultValue(
  FabricCore::Context context,
  FTL::CStrRef typeName
  );

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUIUtil__
