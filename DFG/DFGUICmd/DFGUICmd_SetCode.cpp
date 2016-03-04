//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetCode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetCode::appendDesc( QString &desc )
{
  desc += "Modify code of function ";
  AppendDesc_String( getExecPath(), desc );
}

void DFGUICmd_SetCode::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_code.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_SetCode::invoke(
  FTL::CStrRef code,
  unsigned &coreUndoCount
  )
{
  getExec().setCode( code.c_str() );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
