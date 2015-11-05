// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_SceneHub_Cmd__
#define __FABRICUI_SceneHub_Cmd__

#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <FTL/StrRef.h>
#include <QtGui/QUndoCommand>
#include <FabricUI/SceneHub/macros.h>
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SHCmd
    {
      public:
        /// Constructs a command.
        /// \param cmdManager A reference to the SGCmdManager (kl).
        SHCmd(FabricCore::RTVal &cmdManager) : 
          m_cmdManager(cmdManager), 
          m_coreUndoCount(0), 
          m_state(State_New) {};

        ~SHCmd() {};

        /// Does nothing (don't call the command in KL).
        void doit() {
          assert( m_state == State_New );
          ++m_coreUndoCount;
          m_state = State_Done;
        };

        /// Undoes the command.
        void undo() {
          assert( m_state == State_Done || m_state == State_Redone );
          m_state = State_Undone;
          FABRIC_TRY("SHCmd::undo", 
            for ( unsigned i = 0; i < m_coreUndoCount; ++i )
              FabricCore::RTVal res = m_cmdManager.callMethod("Boolean", "undo", 0, 0);
          );
        };

        /// Redoes the command.
        void redo() {
          assert( m_state = State_Undone );
          m_state = State_Redone;          
          FABRIC_TRY("SHCmd::redo", 
            for ( unsigned i = 0; i < m_coreUndoCount; ++i )
              FabricCore::RTVal res = m_cmdManager.callMethod("", "redo", 0, 0);
          );
        };

        /// Undoes the commands
        std::string getDesc() {
          assert(wasInvoked());
          return m_desc;
        };

        /// Undoes the commands
        void setDesc(std::string desc) {
          m_desc = desc;
        };

      private:
        /// Checks if the command has been already applied.
        bool wasInvoked() const { return m_state != State_New; };
 
        enum State {
          State_New,
          State_Done,
          State_Undone,
          State_Redone
        };

        std::string m_desc;
        FabricCore::RTVal m_cmdManager;
        unsigned m_coreUndoCount;
        State m_state;
    };
  };
};

#endif // __FABRICUI_SceneHub_Cmd__
