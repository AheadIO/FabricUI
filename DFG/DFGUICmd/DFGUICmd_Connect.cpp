//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Connect.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_Connect::appendDesc( std::string &desc )
{
  desc += FTL_STR("Connect ");
  appendDesc_Path( m_srcPath, desc );
  desc += FTL_STR(" to ");
  appendDesc_Path( m_dstPath, desc );
}

void DFGUICmd_Connect::invoke( unsigned &coreUndoCount )
{
  getExec().connectTo(
    m_srcPath.c_str(),
    m_dstPath.c_str()
    );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
