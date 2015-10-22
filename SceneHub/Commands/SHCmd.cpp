//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include "SHCmd.h"
#include <assert.h>
#include <iostream>

using namespace FabricUI;
using namespace FabricUI::SceneHub;
 
void SHCmd::doit()
{
  assert( m_state == State_New );
  m_state = State_Done;

  invoke( m_coreUndoCount );
}

void SHCmd::undo()
{
  assert( m_state == State_Done || m_state == State_Redone );
  m_state = State_Undone;
  std::cerr << "SHCmd::undo()" << std::endl;
  //for ( unsigned i = 0; i < m_coreUndoCount; ++i )
  //  m_host.maybeUndo();
}

void SHCmd::redo()
{
  assert( m_state = State_Undone );
  m_state = State_Redone;
  std::cerr << "SHCmd::redo()" << std::endl;

  //for ( unsigned i = 0; i < m_coreUndoCount; ++i )
  //  m_host.maybeRedo();
}

