//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_SetPortDefaultValue.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_SetPortDefaultValue::appendDesc( QString &desc )
{
  desc += "Set value of ";
  appendDesc_PortPath( m_portPath, desc );
}

void DFGUICmd_SetPortDefaultValue::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_portPath.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_SetPortDefaultValue::invoke(
  FTL::CStrRef portPath,
  unsigned &coreUndoCount
  )
{
  getExec().setPortDefaultValue( portPath.c_str(), m_value, true );
  ++coreUndoCount;
}

FABRIC_UI_DFG_NAMESPACE_END
