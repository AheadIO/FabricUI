/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include "SHCmd.h"
#include "SHCmdHandler_QUndo.h"
#include <FTL/OwnedPtr.h>

using namespace FabricUI;
using namespace FabricUI::SceneHub;

class SHCmdHandler_QUndo::WrappedCmd : public QUndoCommand
{
  public:
    WrappedCmd( SHCmd *shCmd )
      : QUndoCommand()
      , m_shCmd( shCmd )
      , m_didit( false )
    {
    }

  protected:

    virtual void redo()
    {
      try
      {
        if ( m_didit )
        {
          m_shCmd->redo();
        }
        else
        {
          m_didit = true;
          m_shCmd->doit();
          QUndoCommand::setText( m_shCmd->getDesc().c_str() );
        }
      }
      catch ( FabricCore::Exception e )
      {
        printf(
          "Caught FabricCore::Exception: %s\n",
          e.getDesc_cstr()
          );
      }
    }

    virtual void undo()
    {
      try
      {
        assert( m_didit );
        m_shCmd->undo();
      }
      catch ( FabricCore::Exception e )
      {
        printf(
          "Caught FabricCore::Exception: %s\n",
          e.getDesc_cstr()
          );
      }
    }

  private:

    FTL::OwnedPtr<SHCmd> m_shCmd;
    bool m_didit;
};
