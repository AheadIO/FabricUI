//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd.h>

#include <assert.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd::doit()
{
  assert( m_state == State_New );
  m_state = State_Done;

  invoke( m_coreUndoCount );
}

void DFGUICmd::undo()
{
  assert( m_state == State_Done || m_state == State_Redone );
  m_state = State_Undone;
  
  for ( unsigned i = 0; i < m_coreUndoCount; ++i )
    m_host.maybeUndo();
}

void DFGUICmd::redo()
{
  assert( m_state = State_Undone );
  m_state = State_Redone;

  for ( unsigned i = 0; i < m_coreUndoCount; ++i )
    m_host.maybeRedo();
}

FABRIC_UI_DFG_NAMESPACE_END
