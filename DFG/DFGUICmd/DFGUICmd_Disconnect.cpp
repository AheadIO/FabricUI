//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Disconnect.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_Disconnect::appendDesc( std::string &desc )
{
  desc += FTL_STR("Disconnect ");
  appendDesc_Path( m_srcPath, desc );
  desc += FTL_STR(" from ");
  appendDesc_Path( m_dstPath, desc );
}

void DFGUICmd_Disconnect::invoke( unsigned &coreUndoCount )
{
  getExec().disconnectFrom(
    m_srcPath.c_str(),
    m_dstPath.c_str()
    );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
