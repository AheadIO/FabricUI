//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd.h>

#include <assert.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd::doit()
{
  assert( m_state == State_New );

  try
  {
    invoke( m_coreUndoCount );
  }
  catch ( FabricCore::Exception e )
  {
    printf(
      "Caught FabricCore::Exception: %s\n",
      e.getDesc_cstr()
      );
  }

  m_state = State_Done;
}

void DFGUICmd::undo()
{
  assert( m_state == State_Done || m_state == State_Redone );

  try
  {
    for ( unsigned i = 0; i < m_coreUndoCount; ++i )
      m_host.maybeUndo();
  }
  catch ( FabricCore::Exception e )
  {
    printf(
      "Caught FabricCore::Exception: %s\n",
      e.getDesc_cstr()
      );
  }

  m_state = State_Undone;
}

void DFGUICmd::redo()
{
  assert( m_state = State_Undone );

  try
  {
    for ( unsigned i = 0; i < m_coreUndoCount; ++i )
      m_host.maybeRedo();
  }
  catch ( FabricCore::Exception e )
  {
    printf(
      "Caught FabricCore::Exception: %s\n",
      e.getDesc_cstr()
      );
  }

  m_state = State_Redone;
}

FABRIC_UI_DFG_NAMESPACE_END
