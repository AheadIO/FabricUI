//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetRefVarPath.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetRefVarPath::appendDesc( QString &desc )
{
  desc += "Set variable for reference ";
  appendDesc_NodeName( m_refName, desc );
  desc += " to ";
  AppendDesc_String( m_varPath, desc );
}

void DFGUICmd_SetRefVarPath::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_refName.toUtf8().constData(),
    m_varPath.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_SetRefVarPath::invoke(
  FTL::CStrRef refName,
  FTL::CStrRef varPath,
  unsigned &coreUndoCount
  )
{
  getExec().setRefVarPath( refName.c_str(), varPath.c_str(), true );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
