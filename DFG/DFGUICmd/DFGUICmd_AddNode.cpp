// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddNode.h>

#include <FTL/JSONEnc.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddNode::invoke( unsigned &coreUndoCount )
{
  m_actualNodeName = invokeAdd( coreUndoCount );

  std::string posJSONString;
  {
    FTL::JSONEnc<std::string> je( posJSONString, FTL::JSONFormat::Packed() );
    FTL::JSONObjectEnc<std::string> joe( je );
    {
      FTL::JSONEnc<std::string> xJE( joe, FTL_STR("x") );
      FTL::JSONFloat64Enc<std::string> xJFE( xJE, getPos().x() );
    }
    {
      FTL::JSONEnc<std::string> yJE( joe, FTL_STR("y") );
      FTL::JSONFloat64Enc<std::string> yJFE( yJE, getPos().y() );
    }
  }
  getExec().setNodeMetadata(
    m_actualNodeName.c_str(),
    "uiGraphPos",
    posJSONString.c_str(),
    true // canUndo
    );
  ++coreUndoCount;

  getBinding().execute();
}

FABRIC_UI_DFG_NAMESPACE_END
