// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_SceneHub_Cmd__
#define __FABRICUI_SceneHub_Cmd__

#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <FTL/StrRef.h>
#include <QtGui/QUndoCommand>
#include <FabricUI/SceneHub/macros.h>
#include <FabricUI/SceneHub/SHCmdHandler.h>

namespace FabricUI
{
  namespace SceneHub
  {
    struct SHCmdParam {
      std::string name;
      std::string value;
    };

    struct SHCmdDescription {
      std::string name;
      std::vector<SHCmdParam> params;

      std::string asString() {
        std::string str = "name " + name + "\n";
        for(unsigned i=0; i<params.size(); ++i)
          str += "params[" + std::to_string(i) + "] " + params[i].name + " " + params[i].value + "\n";
        return str;
      }

      void print() {
        std::cerr << asString() << std::endl;
      }
    };

    class SHCmd
    {
      public:
        SHCmd(FabricCore::RTVal cmdManager) : 
          m_cmdManager(cmdManager), 
          m_coreUndoCount(0), 
          m_state(State_New) {};

        ~SHCmd() {};

        void doit() {
          assert( m_state == State_New );
            ++m_coreUndoCount;
            std::cerr << "SHCmd::doit() " <<   m_coreUndoCount << std::endl;
            m_state = State_Done;
          };

        void undo() {
          assert( m_state == State_Done || m_state == State_Redone );
          m_state = State_Undone;
          std::cerr << "SHCmd::undo() " << m_coreUndoCount << std::endl;

          FABRIC_TRY("SHCmd::undo", 
            for ( unsigned i = 0; i < m_coreUndoCount; ++i )
              FabricCore::RTVal res = m_cmdManager.callMethod("Boolean", "undo", 0, 0);
          );
        };

        void redo() {
          assert( m_state = State_Undone );
          m_state = State_Redone;
          std::cerr << "SHCmd::redo() " << m_coreUndoCount << std::endl;
          
          FABRIC_TRY("SHCmd::redo", 
            for ( unsigned i = 0; i < m_coreUndoCount; ++i )
              FabricCore::RTVal res = m_cmdManager.callMethod("", "redo", 0, 0);
          );
        };

        SHCmdDescription getDesc() {
          assert(wasInvoked());
          return m_cmdDescription;
        };

        void setDesc(SHCmdDescription cmdDescription) {
          m_cmdDescription = cmdDescription;
        };

      private:
        bool wasInvoked() const { return m_state != State_New; };
 
        enum State {
          State_New,
          State_Done,
          State_Undone,
          State_Redone
        };

        SHCmdDescription m_cmdDescription;
        FabricCore::RTVal m_cmdManager;
        unsigned m_coreUndoCount;
        State m_state;
    };
  };
};

#endif // __FABRICUI_SceneHub_Cmd__
