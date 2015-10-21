// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_SceneHub_Cmd__
#define __FABRICUI_SceneHub_Cmd__

#include "SHCmdHandler.h"
#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <QtGui/QUndoCommand>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHCmd
    {
    public:

      SHCmd(
        FabricCore::Client client
        )
        : m_client( client )
        , m_coreUndoCount( 0 )
        , m_state( State_New )
        {}
      virtual ~SHCmd() {}

      virtual void doit();

      virtual void undo();

      virtual void redo();

      std::string getDesc()
      {
        assert( wasInvoked() );
        std::string desc;
        desc += FTL_STR("SceneHub: ");
        appendDesc( desc );
        return desc;
      }

    protected:

      bool wasInvoked() const { return m_state != State_New; };

      virtual void appendDesc( std::string &desc ) = 0;

      virtual void invoke( unsigned &coreUndoCount ) = 0;

      static void AppendDesc_String(
        FTL::CStrRef string,
        std::string &desc
        )
      {
        desc += '\'';
        desc += string;
        desc += '\'';
      };

    private:

      enum State {
        State_New,
        State_Done,
        State_Undone,
        State_Redone
      };

      FabricCore::Client m_client;
      unsigned m_coreUndoCount;
      State m_state;
    };
  };
};

#endif // __FABRICUI_SceneHub_Cmd__
