//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_ReorderPorts.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_ReorderPorts::appendDesc( std::string &desc )
{
  desc += FTL_STR("Reordered ports [");
  char buffer[16];

  for(size_t i=0;i<m_indices.size();i++)
  {
    if(i > 0)
      desc += FTL_STR(",");
    sprintf(buffer, "%d", (int)m_indices[i]);
    desc += buffer;
  }

  desc += FTL_STR("]");
}

void DFGUICmd_ReorderPorts::invoke( unsigned &coreUndoCount )
{
  getExec().reorderExecPorts(m_indices.size(), &m_indices[0]);
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
