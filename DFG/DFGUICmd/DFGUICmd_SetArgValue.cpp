//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetArgValue.h>
#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetArgValue.h>
#include <Persistence/RTValToJSONEncoder.hpp>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetArgValue::appendDesc( QString &desc )
{
  desc += "Set value of ";
  appendDesc_ArgName( m_argName, desc );
}

void DFGUICmd_SetArgValue::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_argName.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_SetArgValue::invoke(
  FTL::CStrRef argName,
  unsigned &coreUndoCount
  )
{
  getBinding().setArgValue( argName.c_str(), m_value, true );

  // Automatically set as "persistable" arg values that were explicitly set by the user
  // NOTE: metadata additions are not properly undone, however in this case it's not a big issue
  //       since we mostly want to avoid "too big values" like meshes to be persisted, which
  //       shouldn't be the case here.
  getBinding().getExec().setExecPortMetadata(
    argName.c_str(),
    DFG_METADATA_UIPERSISTVALUE,
    "true",
    false
    );

  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
