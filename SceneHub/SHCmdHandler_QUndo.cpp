/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/DFG/DFGUICmd/DFGUICmds.h>
#include <FabricUI/DFG/DFGUICmdHandler_QUndo.h>

using namespace FabricUI;
using namespace FabricUI::SceneHub;

class DFGUICmdHandler_QUndo::SHWrappedCommand : public QUndoCommand
{
  public:
    SHWrappedCommand( DFGUICmd *dfgUICmd )
      : QUndoCommand()
      , m_dfgUICmd( dfgUICmd )
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
          m_dfgUICmd->redo();
        }
        else
        {
          m_didit = true;
          m_dfgUICmd->doit();
          QUndoCommand::setText( m_dfgUICmd->getDesc().c_str() );
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
        m_dfgUICmd->undo();
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

    FTL::OwnedPtr<DFGUICmd> m_dfgUICmd;
    bool m_didit;
};
